#ifndef _GAME_INTERFACE_H_
#define _GAME_INTERFACE_H_

#include <boost/python.hpp>
#include "iscript.h"

#include "igame.h"
#include <vector>

namespace script {

class ScriptGame
{
	game::IGamePtr _game;
public:
	ScriptGame(const game::IGamePtr& game);

	std::string getKeyValue(const std::string& key);
	std::string getRequiredKeyValue(const std::string& key);
};

class GameInterface :
	public IScriptInterface
{
public:
	// Redefine the path list to be of type std::vector
	typedef std::vector<std::string> PathList;

	std::string getModPath();
	ScriptGame currentGame();
	PathList getVFSSearchPaths();

	// IScriptInterface implementation
	void registerInterface(boost::python::object& nspace);
};
typedef boost::shared_ptr<GameInterface> GameInterfacePtr;

} // namespace script

#endif /* _GAME_INTERFACE_H_ */