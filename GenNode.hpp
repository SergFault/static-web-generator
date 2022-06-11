#include <filesystem>

/* class encapsulating logic for
 * checking input,
 * converting input path to output path
 * converting gem to html
 * copy input to output
*/

#ifndef GEN_GENNODE_HPP
#define GEN_GENNODE_HPP

namespace fs = std::filesystem;

class GenNode {
	/* string constants representing extensions */
	static const std::string GMI_EXTENSION;
	static const std::string HTML_EXTENSION;

	/* enumeration representing gemtext tags */
	enum GemTag {
		Heading,
		SubHeading,
		SubSubHeading,
		List,
		Blockquotes,
		Uri
	};


public:
	GenNode(const fs::directory_entry &directoryEntry,
			const fs::path &in_path,
			const fs::path &out_path) noexcept;

	/* start process of generating from this node*/
	GenNode &commit();

	GenNode() noexcept = default;
	GenNode(GenNode &&) noexcept ;
	~GenNode() noexcept = default;
	GenNode(const GenNode &) noexcept = default;
	GenNode & operator=(const GenNode &) noexcept;
	GenNode & operator=(GenNode &&) noexcept;

private:
	/* A line which starts with ``` (i.e. with three backticks) tells the client
	 * to toggle between its ordinary parsing mode, and "preformatted mode".*/
	bool performance_mode = false;

	/* dir entry to process */
	fs::directory_entry directoryEntry;

	/* complete path for the current output file (for current directory entry) */
	fs::path out_file_complete_path;

	/* source directory to copy/converting FROM */
	fs::path in_dir_path;

	/* destination directory to copy/converting TO */
	fs::path out_dir_path;

	/* transforms gemtext string to html */
	std::string &gemStrToHtml(std::string &str);

	static std::string getCompleteOutPath(const fs::path &out_path,
										  const fs::path &in_path,
										  const fs::directory_entry &entry);


	static bool is_gmi(fs::directory_entry &dir_entry);
	static bool hasTag(const std::string &tag, const std::string &str);
	static std::string &stripGemId(const std::string &tag, std::string &str);
	static void placeTag(GenNode::GemTag gemTag, std::string &str);
	static void trim(std::string &str);
	friend std::ostream &operator<<(std::ostream &os, const GenNode &gn);
};

#endif
