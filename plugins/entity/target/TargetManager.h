#ifndef _ENTITY_TARGETMANAGER_H_
#define _ENTITY_TARGETMANAGER_H_

#include <map>
#include <string>
#include "Target.h"

namespace entity {

/**
 * greebo: This singleton TargetManager keeps track of all
 *         Target objects in the current scene. 
 *
 * TargetKey classes acquire a named TargetPtr by calling getTarget(). This 
 * always succeeds - if the named Target is not found, a new, empty one 
 * is created.
 *
 * The TargetableInstances report to this manager as soon as 
 * their name is changed or set. This will associate an empty Target object
 * with an actual scene::Instance.
 *
 *
 *          Target object (can be empty)
 *                   ________
 *                  /        \
 *                  |        |
 * TargetKey ----->>|    -------->> holds scene::Instance* (==NULL, if empty)
 *                  |        |
 *                  \________/
 */
class TargetManager
{
	// The list of all named Target objects
	typedef std::map<std::string, TargetPtr> TargetList;
	TargetList _targets;

	// An empty Target (this is returned if an empty name is requested)
	TargetPtr _emptyTarget;

	// Private constructor
	TargetManager();
public:
	// Accessor to the singleton instance
	static TargetManager& Instance();

	/**
	 * greebo: Returns the Target with the given name.
	 *         
	 * This never returns NULL, a Target is created if it doesn't exist yet.
	 */
	TargetPtr getTarget(const std::string name);

	/**
	 * greebo: Associates the Target with the given name
	 *         to the given scene::Instance.
	 *
	 * The Target will be created if it doesn't exist yet.
	 */
	void associateTarget(const std::string& name, scene::Instance* instance);

	/**
	 * greebo: Disassociates the Target from the given name.
	 */
	void clearTarget(const std::string& name);
};

} // namespace entity

#endif /* _ENTITY_TARGETMANAGER_H_ */