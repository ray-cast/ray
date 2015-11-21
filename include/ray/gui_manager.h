/*!
	@file
	@author		George Evmenov
	@date		07/2009
*/

#ifndef MYGUI_OPENGL3_DATA_MANAGER_H_
#define MYGUI_OPENGL3_DATA_MANAGER_H_

#include "MyGUI_Prerequest.h"
#include "MyGUI_DataManager.h"
#include <ray/platform.h>

_NAME_BEGIN

class GuiResManager : public MyGUI::DataManager
{
public:
	GuiResManager();

	void initialise();
	void shutdown();

	static GuiResManager& getInstance()
	{
		return *getInstancePtr();
	}
	static GuiResManager* getInstancePtr()
	{
		return static_cast<GuiResManager*>(MyGUI::DataManager::getInstancePtr());
	}

	/** @see DataManager::getData(const std::string& _name) */
	virtual MyGUI::IDataStream* getData(const std::string& _name);

	/** @see DataManager::freeData */
	virtual void freeData(MyGUI::IDataStream* _data);

	/** @see DataManager::isDataExist(const std::string& _name) */
	virtual bool isDataExist(const std::string& _name);

	/** @see DataManager::getDataListNames(const std::string& _pattern) */
	virtual const MyGUI::VectorString& getDataListNames(const std::string& _pattern);

	/** @see DataManager::getDataPath(const std::string& _name) */
	virtual const std::string& getDataPath(const std::string& _name);

/*internal:*/
	void addResourceLocation(const std::string& _name, bool _recursive);

private:
	struct ArhivInfo
	{
		std::wstring name;
		bool recursive;
	};
	typedef std::vector<ArhivInfo> VectorArhivInfo;
	VectorArhivInfo mPaths;

	bool mIsInitialise;
};

_NAME_END

#endif
