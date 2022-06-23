#include "stdafx.h"
#include "ZEmoji.h"


ZEmoji::ZEmoji()
{

}

ZEmoji::~ZEmoji()
{

}

ZEmoji* ZEmoji::GetInstance()
{
	static ZEmoji instance;
	return &instance;
}

void ZEmoji::Destroy()
{
	m_EmojiList.clear();
}

bool ZEmoji::ParseXml(const char* fileName, MZFileSystem* fileSystem)
{
	rapidxml::xml_document<> doc;
	MZFile mzf;


	if (!mzf.Open(fileName,fileSystem))
		return false;

	char* buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer, mzf.GetLength());

	if (!doc.parse<0>(buffer)) {
		delete[] buffer;
		doc.clear();
		mzf.Close();
		return false;
	}

	rapidxml::xml_node<>* rootNode = doc.first_node();
	for (rapidxml::xml_node<>* itor = rootNode->first_node(); itor; itor = itor->next_sibling())
	{
		if (itor->name()[0] == '#')
			continue;
		if (_stricmp(itor->name(), "EMOJI") == 0) {
			Emoji node;

			if (!ParseXml_EmojiTag(itor, node))
			{

				mlog("Error parsing emoji tag\n");
				return false;
			}
			m_EmojiList.push_back(node);

		}
	}
	delete[] buffer;
	doc.clear();
	mzf.Close();

	return true;
}

bool ZEmoji::ParseXml_EmojiTag(rapidxml::xml_node<>* xmlNode, Emoji& node)
{
	node.id = 0;
	strcpy(node.name, xmlNode->first_attribute("name")->value());
	strcpy(node.filename ,xmlNode->first_attribute("filename")->value());
	strcpy(node.input, xmlNode->first_attribute("input")->value());

	return true;
}

bool const ZEmoji::Find(const char* text, const char* emojiInput)
{
	char emoji[4];
	for (auto& elem  = m_EmojiList.begin(); elem != m_EmojiList.end(); ++elem)
	{
		for (int i = 0; i <= static_cast<int>(strlen(elem->input)); ++i)
		{
			for (int j = 0; j < static_cast<int>(strlen(text)); ++j)
			{
				if (text[j] == elem->input[i])
					emoji[i] = text[j];
			}
			if (i == static_cast<int>(strlen(elem->input)))
			{
				if (strstr(emoji, elem->input))
				{
					emojiInput = emoji;
					return true;
				}
			}
		}
	}
	return false;
}

void ZEmoji::AddEmoji(const char* inputText, int& nX,int& nY)
{
	for (auto& itor  = m_EmojiList.begin(); itor != m_EmojiList.end(); ++itor)
	{
		//if (emoji.input.find(inputText) != std::string::npos)
		{
			std::string filePos = "interface/default/emojis/";
			filePos.append(itor->filename);
			MBitmapR2* bitMap = new MBitmapR2;// MBitmapManager::Get(emoji.filename);
			bitMap->Create(filePos.c_str(), RGetDevice(), filePos.c_str());
			bitMap->Draw(nX, nY, nX, nY, 0, 0, 0, 0, 0);
		}

	}
}