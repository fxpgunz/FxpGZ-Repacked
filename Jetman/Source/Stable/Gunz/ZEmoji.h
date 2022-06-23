#pragma once

struct Emoji
{
	int id;
	char name[24];
	char filename[28];
	char input[2];
};

class ZEmoji
{
public:
	vector<Emoji> m_EmojiList;
	ZEmoji();
	~ZEmoji();

	static ZEmoji* GetInstance();

	void Destroy();
	void AddEmoji(const char* input, int& nX,int& nY);

	bool ParseXml(const char* fileName, MZFileSystem* fileSystem);
	bool ParseXml_EmojiTag(rapidxml::xml_node<>* xmlNode, Emoji& node);

	bool const Find(const char* text, const char* emojiInput);


}; inline ZEmoji* ZGetEmojis() { return ZEmoji::GetInstance(); }