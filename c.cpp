#include <fstream>
#include <ios>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "c.h"

int main(){
    int user_option;
   
    while(true){
        menuMain();
        while(true){
            std::cout << "Enter your choice here (1-3): ";

            if(!(std::cin >> user_option)){
                std::cin.clear();
                std::cout << "Invalid input, please enter an integer (1-3).\n" << std::endl;
                std::cin.ignore(1000,'\n');
                
                continue;
            }
            if(!(user_option > 0 && user_option < 4)){
                std::cin.clear();
                std::cout << "Invalid option, please enter an integer (1-3).\n" << std::endl;
                std::cin.ignore(1000,'\n');
                continue;
            }

            std::cout << std::endl;
            break;
        }

        std::string FileName;
        Format result;
        std::fstream fileObj;
        Format userFormat;
        // We have sanitized user input

        try{
            switch(user_option){
                case 1: // Check Newline Format
                    // Try getting a filename
                    try{
                        FileName = GetFileName();
                        fileObj.open(FileName);
                        result = GetNewlineFormat(fileObj);
                        switch(result){
                            case Format::CR:
                                std::cout << "This file is in Carriage Return format.\n" << std::endl;
                                break;
                            case Format::LF:
                                std::cout << "This file is in Line Feed format.\n" << std::endl;
                                break;
                            case Format::CRLF:
                                std::cout << "This file is in Carriage Return Line Feed format.\n" << std::endl;
                                break;
                            default:
                                std::cout << "Format could not be determined, or no newlines were present.\n" << std::endl;
                                break;
                        }

                        // Close file. 
                        fileObj.close();
                    }catch(std::string File_Name){
                        // "Continue" loop, starts over at main menu.
                        std::cout << "You entered: " << File_Name << "\n" << std::endl;
                        continue;
                    }catch(std::ios::failure fail){
                        std::cout << fail.what() << "\n" << std::endl;
                        continue;
                    }
                    break;
                case 2: // Convert text file newline format.
                    FileName =  GetFileName();
                    userFormat = GetUserFormat();
                    ConvertNewlineFormat(userFormat, FileName);
                    break;
                case 3: // quit program.
                    std::cout << "Exiting...";
                    return 0;
                    break;
                default:
                    std::cout << "Default chosen." << std::endl;
            }
        }catch(std::ios::failure fail){ // ios::failure, std::string (quit),  
            std::cout << "Error: " << fail.what() << std::endl;
            std::cout << "Sending to main menu.\n" << std::endl;
        }catch(std::string FileName){
            std::cout << "You entered: " << FileName << std::endl;
            std::cout << "Sending to main menu.\n" << std::endl;
        }
    }

    return 0;
}

std::fstream OpenFileRead(std::string File_Name){ // Return an opened (found) file, or throw exception if not able to.
    // Input, binary, start at eof
    std::fstream file = std::fstream(File_Name,std::ios::binary | std::ios::in);

    // Check for issues:
    //  - File Not Found
    //  - EOF
    if(!file.is_open()){ // File not opened / File not founds
        std::cout << "File '" << File_Name << "' could not be opened." << std::endl;
        throw std::ios::failure{"File not found."};
    }

    if(file.eof()){
        std::cout << "File '" << File_Name << "' is empty." << std::endl;
        throw std::ios::failure{"File is empty."};
    }

    // // For debugging, read current state directly from object
    // std::ios_base::iostate object = file.rdstate();

    // std::cout << "File rdstate after opening: " << object << std::endl;
    // std::cout << "File goodbit after opening: " << std::ios_base::goodbit << std::endl;
    // std::cout << "File badbit after opening: " << std::ios_base::badbit << std::endl;
    // std::cout << "File eofbit after opening: " << std::ios_base::eofbit << std::endl;
    // std::cout << "File failbit after opening: " << std::ios_base::failbit << std::endl;

    return file;
}

std::fstream OpenFileWrite(std::string File_Name){ // Return an opened (found) file, or throw exception if not able to.
    // Input, binary, start at eof
    std::fstream file = std::fstream(File_Name,std::ios::binary | std::ios::out | std::ios::in);

    // Check for issues:
    //  - File Not Found
    //  - EOF
    if(!file.is_open()){ // File not opened / File not founds
        std::cout << "File '" << File_Name << "' could not be opened." << std::endl;
        throw std::ios::failure{"File not found."};
    }

    if(file.eof()){
        std::cout << "File '" << File_Name << "' is empty." << std::endl;
        throw std::ios::failure{"File is empty."};
    }

    // // For debugging, read current state directly from object
    // std::ios_base::iostate object = file.rdstate();

    // std::cout << "File rdstate after opening: " << object << std::endl;
    // std::cout << "File goodbit after opening: " << std::ios_base::goodbit << std::endl;
    // std::cout << "File badbit after opening: " << std::ios_base::badbit << std::endl;
    // std::cout << "File eofbit after opening: " << std::ios_base::eofbit << std::endl;
    // std::cout << "File failbit after opening: " << std::ios_base::failbit << std::endl;

    return file;
}

// Checks first Newline it can find, returns Format::IN if it can't find valid newline. 
Format GetNewlineFormat(std::fstream& file){
    
    // Read each character until program finds LF,CR, or CRLF
    char curr = 0; // Curr char
    char prev = 0; // Prev char (in case of /r/n)
    
    bool CRLF = false;
    bool LF = false;
    bool CR = false;

    while(file.get(curr)){ // Write next character into curr
        // \n, \r, \r\n
        // if we get \n first, then Line Feed guaranteed.
        // if we get \r first, then it MAY be \r or \r\n
        if(curr == '\n'){
            if(prev == '\r'){
                // CRLF guaranteed.
                CRLF = true;
            }
            // Line Feed guaranteed.
            LF = true;
        }else if(curr == '\r'){
            CR = true; // MAYBE
        }

        prev = curr;
    }

    if(CRLF){
        return Format::CRLF;
    }else if(LF){
        return Format::LF;
    }else if(CR && !CRLF){
        return Format::CR;
    }else{
        return Format::IN;
    }
}

// Runs "Check Newline Format" command. Handles file exceptions.
Format RunFormatDetection(std::string File_Name){
    
    std::fstream file = OpenFileRead(File_Name);
    
    // Pass to function to get file format
    Format format = GetNewlineFormat(file);

    file.close();

    switch((int) format){
        case 0: // LF
            std::cout << "This file uses Line Feed format!" << std::endl;
            return Format::LF;
            break;
        case 1: // CRLF
            std::cout << "This file uses Carriage Return Line Feed format!" << std::endl;
            return Format::CRLF;
            break;
        case 2: //CR
            std::cout << "This file uses Carriage Return format!" << std::endl;
            return Format::CR;
            break;
        case 3:
            std::cout << "This file has no newline character or has an invalid format!" << std::endl;
            return Format::IN;
            break;
    }
    

    return Format::IN;
}

std::string GetFileName(){
    
    std::string File_Name;
    std::cout << "Please enter file name here: ";
    std::cin >> File_Name;

    // Allow user to exit this menu.
    if(File_Name == "quit" || File_Name == "Quit"){
        throw (File_Name);
    }

    return File_Name;
}    

void menuMain(){
    std::cout << "This program can detect and replace newline formats within text files." << std::endl;
    std::cout << "Please Note: This program does not check file types before running.\nPlease ensure that you are only entering TEXT files and not other file types." << std::endl;

    std::cout << "Options:" << std::endl;
    std::cout << "1.) Check newline format" << std::endl;
    std::cout << "2.) Convert text file newline format" << std::endl;
    std::cout << "3.) Quit" << std::endl;
}

Format ConvertNewlineFormat(Format NewFormat, std::string File_Name){
    // We need to get the initial format of the file (if any) using a read-only file-object.
    std::fstream file = OpenFileWrite(File_Name);

    // Pass to function to get file format, doesn't throw any exceptions. Should end program if it does somehow fail/throw an error.
    // This should help prevent memory leaks by having it crash rather than handled.
    Format format = GetNewlineFormat(file);

    // ENSURE that file closes HERE
    file.close();

    // Get appropriate newline character:
    char newLine_Format;
    switch(NewFormat){
        case Format::LF:
            newLine_Format = '\n';
            break;
        case Format::CRLF:
            // Nothing, if-statement below handles it. 
            break;
        case Format::CR:
            newLine_Format = '\r';
            break;
        default:
            // Exit function if format is invalid.
            throw std::ios::failure{"Invalid newline format detected, or no newlines detected."};
            break;
    }

    // Get what format the file currently is
    char currLine_Format;
    switch(format){
        case Format::LF:
            currLine_Format = '\n';
            break;
        case Format::CRLF:
            // Nothing, if-statement below handles it.
            break;
        case Format::CR:
            currLine_Format = '\r';
            break;
        default:
            // Exit function if format is invalid.
            throw std::ios::failure{"Invalid newline format detected, or no newlines detected."};
            break;
    }


    // End now if File and chosen new format is the same.
    if(format == Format::CRLF && NewFormat == Format::CRLF){
        std::cout << "File is already CRLF format." << std::endl;
        return Format::CRLF;
    }else if(format == Format::CR && NewFormat == Format::CR){
        std::cout << "File is already CR format." << std::endl;
        return Format::CR;
    }else if(format == Format::LF && NewFormat == Format::LF){
        std::cout << "File is already LF format." << std::endl;
        return Format::LF;
    }
   

    if(format == Format::CRLF && (NewFormat == Format::LF || NewFormat == Format::CR)){
        CRLFToSingle(newLine_Format, File_Name);
    }else if((format == Format::LF || format == Format::CR) && NewFormat == Format::CRLF){
        SingleToCRLF(currLine_Format, File_Name);
    }else{
        // Else its single char to single char
        SingleToSingle(currLine_Format, newLine_Format, File_Name);
    }

    return Format::IN;
}

// Formatting single char newline to CRLF
void SingleToCRLF(char single, std::string File_Name){
    // I need to ADD a new character to account for /r/n (2 chars)
    // Similar idea as CRLFToSingle except we add a character when reading the text to make it CRLF
    
    // We need to get the initial format of the file (if any) using a read-only file-object.
    std::fstream file = OpenFileRead(File_Name);

    char curr;
    size_t pos;

    std::string File_Contents;
    if(single == '\r'){
        // Add a character after '\r' to make CRLF 
        while(file.get(curr)){
            File_Contents += curr;
            if(curr == '\r'){
                // Additionally add \n
                File_Contents += '\n';
            }
        }
    }else if(single == '\n'){
        // When \n is detected, write \r\n instead of just \n
        while(file.get(curr)){
            if(curr == '\n'){
                File_Contents += '\r';
                File_Contents += '\n';
                continue;
            }
            File_Contents += curr;
        }
    }else{
        file.close();
        throw std::ios::failure{"Invalid newline character option"};
    }

    // Now we have the formatted text, close old read file and open up as trunc-write
    file.close();

    std::ofstream ofs;
    ofs.open(File_Name, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    for(char i : File_Contents){
        ofs.put(i);
    }
    // Done.
    ofs.close();
}

// Formatting CRLF to single char newline
void CRLFToSingle(char single, std::string File_Name){
    // I need to DELETE a character to switch from /r/n (2 chars) to a single char /r or /n
    std::fstream file = OpenFileRead(File_Name);

    // Depending on which single char it is "\r" or "\n", we could just delete one character or the other to convert
    char curr;
    size_t pos;
    char Ignore_Char;

    // We should read the file and store it as a single string. 
    // Then we read from that string into the file, ignoring the character that we don't want. 

    std::cout << "Single: " << (int) single << std::endl;
    if(single == '\r'){
        Ignore_Char = '\n';
    }else if(single == '\n'){
        // Erase all "\r" characters
        Ignore_Char = '\r';
    }else{
        // Invalid option.
        file.close();
        std::cout << "CRLF to Single" << std::endl;
        throw std::ios::failure{"Invalid newline character option."};
    }

    // Read from file, write contents of file to a string:
    std::string fileContent;
    while(file.get(curr)){
        fileContent += curr;
    }
    
    // Close file, open up with truncate-option to clear file
    file.close();

    std::ofstream ofs;
    ofs.open(File_Name,std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

    // Read string, ignoring the ignore_char
    for(char i : fileContent){
        if(i != Ignore_Char){
            ofs.put(i);
        }
    }

    // close file, ending this function.
    ofs.close();
}

// Formatting single char to single char
void SingleToSingle(char single, char newSingle, std::string File_Name){
    std::fstream file = OpenFileWrite(File_Name);
    // Replace character.
    char curr;
    size_t pos;
    while(file.get(curr)){
        if(curr == single){
            pos = (size_t) file.tellp();
            file.seekp(pos - 1); 
            file.put(newSingle);
            
        }
    }
    file.close();
    
}

// Grab Format to change text to through user terminal input
Format GetUserFormat(){
    int user_option;

    while(true){
        std::cout << "Which newline format do you want to convert your text file to?" << std::endl;
        std::cout << "1. Carriage Return Line Feed (CRLF)" << std::endl;
        std::cout << "2. Line Feed (LF)" << std::endl;
        std::cout << "3. Carriage Return (CR)" << std::endl;
        std::cout << "Enter here (1-3):";

        if(!(std::cin >> user_option)){
                std::cin.clear();
                std::cout << "Invalid input, please enter an integer (1-3).\n" << std::endl;
                std::cin.ignore(1000,'\n'); 
                continue;
        }
        if(!(user_option > 0 && user_option < 4)){
                std::cin.clear();
                std::cout << "Invalid option, please enter an integer (1-3).\n" << std::endl;
                std::cin.ignore(1000,'\n');
                continue;
        }
        std::cout << std::endl;
        break;
    }

    switch(user_option){
        case 1:
            return Format::CRLF;
            break;
        case 2:
            return Format::LF;
            break;
        case 3:
            return Format::CR;
            break;
        default:
            return Format::IN;
            break;
    }
}