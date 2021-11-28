#include <Config.h>

#include <cctype>
#include <cassert>

cfg::block::~block()
{
	for (auto& block : m_blocks) {
		delete block.value();
	}
}

cfg::block* cfg::block::getBlock(const char* name)
{
	auto pair = m_blocks.find_key(name);
	if (pair != nullptr) {
		return pair->value();
	}
	return nullptr;
}

int cfg::block::getInt(const char* name, int def)
{
	auto pair = m_values.find_key(name);
	if (pair != nullptr && pair->value().getType() == t_int) {
		return pair->value().getInt();
	}
	return def;
}

float cfg::block::getFloat(const char* name, float def)
{
	auto pair = m_values.find_key(name);
	if (pair != nullptr && pair->value().getType() == t_float) {
		return pair->value().getFloat();
	}
	return def;
}

const char* cfg::block::getString(const char* name, const char* def)
{
	auto pair = m_values.find_key(name);
	if (pair != nullptr && pair->value().getType() == t_string) {
		return pair->value().getString();
	}
	return def;
}

void cfg::block::loadFromFile(FILE* fh)
{
	s2::string bufferName;
	s2::string bufferValue;

	bool readingValue = false;

	while (!feof(fh)) {
		int ci = fgetc(fh);
		if (ci == EOF) {
			break;
		}

		char c = (char)ci;

		// Don't care about whitespace
		if (c == ' ' || c == '\t' || c == '\r') {
			continue;
		}

		// Don't care about comments
		if (c == '#') {
			while (!feof(fh)) {
				if (fgetc(fh) == '\n') {
					break;
				}
			}
			continue;
		}

		// Values
		if (isalnum(c) || c == '.') {
			if (readingValue) {
				bufferValue.append(c);
			} else {
				bufferName.append(c);
			}
			continue;
		}

		if (c == '=') {
			readingValue = true;
			continue;
		}

		// Strings
		if (c == '"' && readingValue && bufferValue.len() == 0) {
			bool literal = false;
			while (!feof(fh)) {
				char cc = fgetc(fh);

				if (cc == '\\') {
					literal = true;
					continue;
				}

				if (cc == '"') {
					// End of string, find end of line/file
					while (!feof(fh)) {
						if (fgetc(fh) == '\n') {
							break;
						}
					}

					m_values.set(bufferName, value(bufferValue));
					//printf("Value: \"%s\" = \"%s\"\n", bufferName.c_str(), bufferValue.c_str());

					bufferName = "";
					bufferValue = "";
					readingValue = false;
					break;
				}
				bufferValue.append(cc);
			}
			continue;
		}

		if (c == '\n') {
			if (bufferName == "" && bufferValue == "") {
				continue;
			}

			type guessedType = t_null;
			for (size_t i = 0; i < bufferValue.len(); i++) {
				char cc = bufferValue[i];
				if (cc == '"') {
					guessedType = t_string;
					break;
				} else if (isdigit(cc)) {
					guessedType = t_int;
				} else if (cc == '.' && guessedType == t_int) {
					guessedType = t_float;
					break;
				} else {
					guessedType = t_string;
					break;
				}
			}

			switch (guessedType) {
				case t_int: m_values.set(bufferName, value(atoi(bufferValue))); break;
				case t_float: m_values.set(bufferName, value((float)atof(bufferValue))); break;
				case t_string: m_values.set(bufferName, value(bufferValue)); break;
			}

			if (guessedType == t_null) {
				printf("Unhandled type for value \"%s\"\n", bufferName);
			}
			//printf("Value: \"%s\" = \"%s\" (type %d)\n", bufferName.c_str(), bufferValue.c_str(), (int)guessedType);

			bufferName = "";
			bufferValue = "";
			readingValue = false;
			continue;
		}

		// Blocks
		if (c == '{' && !readingValue) {
			block* newBlock = new block;
			newBlock->loadFromFile(fh);
			m_blocks.set(bufferName, newBlock);
			//printf("Block: \"%s\" (%d values, %d blocks)\n", bufferName.c_str(), (int)newBlock->m_values.len(), (int)newBlock->m_blocks.len());
			bufferName = "";
			continue;
		}

		// End of block
		if (c == '}') {
			break;
		}

		printf("STOP: got character '%c' (%d) (name \"%s\")\n", c, (int)c, bufferName.c_str());
		return;
	}
}

cfg::block* cfg::fromFile(const char* filename)
{
	FILE* fh = fopen(filename, "rb");
	if (fh == nullptr) {
		return nullptr;
	}

	cfg::block* ret = new cfg::block;
	ret->loadFromFile(fh);
	fclose(fh);

	return ret;
}
