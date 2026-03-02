#include <iostream>

enum Format{LF,CRLF,CR,IN};
std::fstream OpenFileRead(std::string File_Name);
Format GetNewlineFormat(std::fstream &file);
Format ConvertNewlineFormat(Format format, std::string File_Name);
std::string GetFileName();
Format RunFormatDetection(std::string File_Name);
void menuMain();
void SingleToCRLF(char single, std::string File_Name);
void CRLFToSingle(char single, std::string File_Name);
void SingleToSingle(char single, char newSingle, std::string File_Name);
Format GetUserFormat();
