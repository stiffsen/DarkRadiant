#include "DifficultySettingsManager.h"

#include "ieclass.h"
#include "ientity.h"
#include "scenelib.h"
#include "iregistry.h"
#include "stream/textstream.h"
#include "DifficultyEntity.h"
#include "DifficultyEntityFinder.h"

namespace difficulty {

DifficultySettingsPtr DifficultySettingsManager::getSettings(int level) {
	for (std::size_t i = 0; i < _settings.size(); i++) {
		if (_settings[i]->getLevel() == level) {
			return _settings[i];
		}
	}
	return DifficultySettingsPtr();
}

void DifficultySettingsManager::loadSettings() {
	loadDefaultSettings();
	loadMapSettings();
}

void DifficultySettingsManager::loadDefaultSettings() {
	// Try to lookup the given entityDef
	IEntityClassPtr eclass = GlobalEntityClassManager().findClass(
		GlobalRegistry().get(RKEY_DIFFICULTY_ENTITYDEF_DEFAULT)
	);

	if (eclass == NULL) {
		globalErrorStream() << "Could not find default difficulty settings entityDef.\n";
		return;
	}

	// greebo: Setup the default difficulty levels using the found entityDef
	int numLevels = GlobalRegistry().getInt(RKEY_DIFFICULTY_LEVELS);
	for (int i = 0; i < numLevels; i++) {
		// Allocate a new settings object
		DifficultySettingsPtr settings(new DifficultySettings(i));

		// Parse the settings from the given default settings entityDef
		settings->parseFromEntityDef(eclass);

		// Store the settings object in the local list
		_settings.push_back(settings);
	}
}

void DifficultySettingsManager::loadMapSettings() {
	// Construct a helper walker
	DifficultyEntityFinder finder;
	GlobalSceneGraph().traverse(finder);

	const DifficultyEntityFinder::EntityList& found = finder.getEntities();

	// Pop all entities into each difficulty setting
	for (DifficultyEntityFinder::EntityList::const_iterator ent = found.begin(); 
		 ent != found.end(); ent++)
	{
		for (std::size_t i = 0; i < _settings.size(); i++) {
			_settings[i]->parseFromMapEntity(*ent);
		}
	}
}

void DifficultySettingsManager::saveSettings() {
	// Locates all difficulty entities
	DifficultyEntityFinder finder;
	GlobalSceneGraph().traverse(finder);

	// Copy the list from the finder to a local list
	DifficultyEntityFinder::EntityList entities = finder.getEntities();

	if (entities.empty()) {
		// Create a new difficulty entity
		std::string eclassName = GlobalRegistry().get(RKEY_DIFFICULTY_ENTITYDEF_MAP);
		IEntityClassPtr diffEclass = GlobalEntityClassManager().findClass(eclassName);

		if (diffEclass == NULL) {
			globalErrorStream() << "[Diff]: Cannot create difficulty entity!\n";
			return;
		}

		// Create and insert a new entity node into the scenegraph root 
		scene::INodePtr entNode = GlobalEntityCreator().createEntity(diffEclass);
		Node_getTraversable(GlobalSceneGraph().root())->insert(entNode);

		// Get the entity of this node
		Entity* entity = Node_getEntity(entNode);

		if (entity == NULL) {
			globalErrorStream() << "[Diff]: Cannot cast node to entity!\n";
			return;
		}

		// Add the entity to the list
		entities.push_back(entity);
	}

	// Clear all difficulty-spawnargs from existing entities
	for (DifficultyEntityFinder::EntityList::const_iterator i = entities.begin();
		 i != entities.end(); i++)
	{
		// Construct a difficulty entity using the raw Entity* pointer
		DifficultyEntity diffEnt(*i);
		// Clear the difficulty-related spawnargs from the entity
		diffEnt.clear();
	}
	
	// Take the first entity
	DifficultyEntity diffEnt(*entities.begin());

	// Cycle through all settings objects and issue save call
	for (std::size_t i = 0; i < _settings.size(); i++) {
		_settings[i]->saveToEntity(diffEnt);
	}
}

} // namespace difficulty