#include <filesystem>

/* encapsulating logic to construct appropriate paths
 * checking input,
 * converting input path to output path
 * converting gem to html
 * copying input to output
*/

#ifndef GEN_GENNODE_HPP
#define GEN_GENNODE_HPP

namespace fs = std::filesystem;

class GenNode {
	/* string constants representing extensions */
	static const std::string GMI_EXTENSION;
	static const std::string HTML_EXTENSION;

	/* enumeration representing gemtext IDs */
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

	/* default is enough for this Class */
	GenNode() noexcept = default;
	GenNode(GenNode &&) noexcept = default;
	~GenNode() noexcept = default;
	GenNode(const GenNode &) noexcept = default;
	GenNode &operator=(const GenNode &) noexcept = default;
	GenNode &operator=(GenNode &&) noexcept  = default;

	/* starts process of converting this node to output file */
	GenNode &commit();

	/* for sake of debug and error messages */
	friend std::ostream &operator<<(std::ostream &os, const GenNode &gn);
private:
	/* A line which starts with ``` (i.e. with three backticks) tells the client
	 * to toggle between its ordinary parsing mode, and "preformatted mode".*/
	bool performance_mode = false;

	/* dir entry to process */
	fs::directory_entry directoryEntry;

	/* complete path for the current output file (for current directory entry) */
	fs::path out_file_complete_path;

	/* source base directory to copy/converting FROM */
	fs::path in_dir_path;

	/* base destination directory to copy/converting TO */
	fs::path out_dir_path;


	/* transforms gemtext string to html */
	std::string &gemStrToHtml(std::string &str);
	/* static util methods */
	static std::string getCompleteOutPath(const fs::path &out_path,
										  const fs::path &in_path,
										  const fs::directory_entry &entry);
	static bool is_gmi(fs::directory_entry &dir_entry);
	static bool hasIndicator(const std::string &tag, const std::string &str);
	std::string &stripGemIndicator(const std::string &tag, std::string &str);
	static void placeTag(GenNode::GemTag gemTag, std::string &str);
	static void trim(std::string &str);
};

#endif
