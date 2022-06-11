#include "GenNode.hpp"
#include <fstream>
#include <string>
#include <iostream>

const std::string GenNode::GMI_EXTENSION = ".gmi";
const std::string GenNode::HTML_EXTENSION = ".html";


GenNode::GenNode(const fs::directory_entry &dir_entry,
				 const fs::path &in_path,
				 const fs::path &out_path) noexcept
{
	directoryEntry = dir_entry;
	out_dir_path = absolute(out_path);
	in_dir_path = absolute(in_path);
	out_file_complete_path = getCompleteOutPath(out_dir_path, in_dir_path,
												dir_entry);
}

GenNode &GenNode::commit()
{
	try
	{
		if (directoryEntry.is_directory())
		{
			fs::create_directories(out_file_complete_path);
		} else if (directoryEntry.is_regular_file())
		{
			if (is_gmi(directoryEntry))
			{
				out_file_complete_path.replace_extension(HTML_EXTENSION);
				std::string in_string;
				std::ifstream fis(directoryEntry.path());
				std::ofstream fos(out_file_complete_path);
				while (std::getline(fis, in_string))
				{
					in_string = gemStrToHtml(in_string);
					fos << in_string << std::endl;
				}
				if (!fis.eof() || fos.bad() || fos.fail())
					throw fs::filesystem_error("write html to file",
											   std::make_error_code(
													   std::errc::io_error));
			} else
				fs::copy_file(directoryEntry.path(), out_file_complete_path,
							  fs::copy_options::update_existing);
		}
	} catch (std::filesystem::filesystem_error &ex)
	{
		std::cout << "Error:\n" << ex.what() << std::endl
				  << "While processing node:\n"
				  << *this;
		throw ex;
	}
	return *this;
}

std::string GenNode::getCompleteOutPath(const fs::path &out_path,
										const fs::path &in_path,
										const fs::directory_entry &entry)
{
	std::string postfix = absolute(entry.path()).string().substr(
			in_path.string().size());
	return
			out_path.string() +
			absolute(entry.path()).string().substr(in_path.string().size());
}

bool GenNode::is_gmi(fs::directory_entry &dir_entry)
{
	return dir_entry.path().extension().string() == GMI_EXTENSION;
}

bool GenNode::hasTag(const std::string &tag, const std::string &str)
{
	size_t str_pos = str.rfind(tag, tag.size() - 1);
	return (str_pos != std::string::npos);
}

std::string &GenNode::stripGemId(const std::string &tag, std::string &str)
{
	return str = str.substr(tag.size());
}

void GenNode::trim(std::string &str)
{
	size_t pos = str.find_first_not_of("\t ");
	if (pos == std::string::npos)
		str = "";
	else
		str = str.substr(pos);
}

void GenNode::placeTag(GenNode::GemTag gemTag, std::string &str)
{
	switch (gemTag)
	{
		case GemTag::Heading :
			str = std::string("<h1>") + str + std::string("</h1>");
			break;
		case GemTag::SubHeading :
			str = std::string("<h2>") + str + std::string("</h2>");
			break;
		case GemTag::SubSubHeading :
			str = std::string("<h3>") + str + std::string("</h3>");
			break;
		case GemTag::List :
			str = std::string("<li>") + str + std::string("</li>");
			break;
		case GemTag::Blockquotes :
			str = std::string("<blockquote>") + str +
				  std::string("</blockquote>");
			break;
		case GemTag::Uri :
		{
			size_t pos;
			std::string uri_link;
			std::string uri_text;
			if ((pos = str.find_first_of(" \t")) != std::string::npos)
			{
				uri_link = str.substr(0, pos);
				str = str.substr(pos);
				trim(str);
				uri_text = str;
			} else
			{
				uri_link = uri_text = str;
			}
			str = std::string("<a href=\"") + uri_link + ("\">") + uri_text +
				  std::string("</a>");
		}
			break;
	}
}

std::string &GenNode::gemStrToHtml(std::string &str)
{
	if (hasTag("'''", str))
	{
		stripGemId("'''", str);
		performance_mode = !performance_mode;
	}

	if (!performance_mode)
	{
		trim(str);
		if (hasTag("###", str))
		{
			stripGemId("###", str);
			trim(str);
			placeTag(SubSubHeading, str);
		} else if (hasTag("##", str))
		{
			stripGemId("##", str);
			trim(str);
			placeTag(SubHeading, str);
		} else if (hasTag("#", str))
		{
			stripGemId("#", str);
			trim(str);
			placeTag(Heading, str);
		} else if (hasTag("*", str))
		{
			stripGemId("*", str);
			trim(str);
			placeTag(List, str);
		} else if (hasTag(">", str))
		{
			stripGemId(">", str);
			trim(str);
			placeTag(Blockquotes, str);
		} else if (hasTag("=>", str))
		{
			stripGemId("=>", str);
			trim(str);
			placeTag(Uri, str);
		} else
		{
			str = std::string("<p>") + str + std::string("</p>");
		}
	}
	return str;
}

GenNode::GenNode(GenNode &&gn) noexcept
{
	performance_mode = std::exchange(gn.performance_mode, false);
	directoryEntry = std::move(gn.directoryEntry);
	out_file_complete_path = std::move(gn.out_file_complete_path);
	in_dir_path = std::move(gn.in_dir_path);
	out_dir_path = std::move(gn.out_dir_path);
}

GenNode &GenNode::operator=(const GenNode &other) noexcept
{
	if (this == &other)
		return *this;
	performance_mode = other.performance_mode;
	directoryEntry = other.directoryEntry;
	out_file_complete_path = other.out_file_complete_path;
	in_dir_path = other.in_dir_path;
	out_dir_path = other.out_dir_path;
	return *this;
}

std::ostream &operator<<(std::ostream &os, const GenNode &gn)
{
	os << "		<<<Generating Node>>>" << std::endl;
	os << "		Performance mode:	" << gn.performance_mode << std::endl;
	os << "		Dir entry:			" << gn.directoryEntry << std::endl;
	os << "		Complete out path:	" << gn.out_file_complete_path
	   << std::endl;
	os << "		In dir path:		" << gn.in_dir_path << std::endl;
	os << "		Out dir path:		" << gn.out_dir_path << std::endl;
	return os;
}

GenNode &GenNode::operator=(GenNode &&gn) noexcept
{
	performance_mode = std::exchange(gn.performance_mode, false);
	directoryEntry = std::move(gn.directoryEntry);
	out_file_complete_path = std::move(gn.out_file_complete_path);
	in_dir_path = std::move(gn.in_dir_path);
	out_dir_path = std::move(gn.out_dir_path);
}
