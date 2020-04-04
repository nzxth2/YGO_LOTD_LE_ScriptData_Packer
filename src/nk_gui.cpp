#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#include "nk_gui.h"

#include "nuklear/nuklear.h"
#include "nuklear/nuklear_gdi.h"
#include "window.h"
#include "filehandling.h"
#include <filesystem>

GdiFont* font;
struct nk_context *ctx;
unsigned int windowWidth;
unsigned int windowHeight;


std::string infoString;
std::unordered_map<Short,std::string> ID_to_name;
std::unordered_map<std::string,Short> name_to_ID;
int checkedDontCheckDeckSize;
std::string startingPath;

struct SCRIPT_BLOCK{
    std::string chara;
    std::string arg1;
    std::string arg2;
    std::string text;
};


void SetupGui(WINDOW_DATA &windowData,unsigned int initialWindowWidth, unsigned int initialWindowHeight){
    font = nk_gdifont_create("Segoe UI", 18);
    windowWidth=initialWindowWidth;
    windowHeight=initialWindowHeight;
    ctx = nk_gdi_init(font, windowData.dc, initialWindowWidth, initialWindowHeight);
    
    
    infoString="";
    checkedDontCheckDeckSize=0;
    startingPath=std::filesystem::current_path().string();
    
}

void HandleInput(WINDOW_DATA &windowData){
        MSG msg;
        nk_input_begin(ctx);
        if (windowData.needs_refresh == 0) {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                windowData.running = 0;
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            windowData.needs_refresh = 1;
        } else windowData.needs_refresh = 0;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                windowData.running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            windowData.needs_refresh = 1;
        }
        nk_input_end(ctx);
        
}

int HandleEvent(const EVENT_DATA &eventData){
    switch (eventData.msg)
    {
	case WM_SIZE:
        windowWidth=LOWORD(eventData.lparam);
        windowHeight=HIWORD(eventData.lparam);
		return nk_gdi_handle_event(eventData.wnd, eventData.msg, eventData.wparam, eventData.lparam);
    default:
        return nk_gdi_handle_event(eventData.wnd, eventData.msg, eventData.wparam, eventData.lparam);
    }
    
}

void HandleGui(){
    if (nk_begin(ctx, "Demo", nk_rect(0, 0, windowWidth, windowHeight),
        0))
    {
        nk_layout_row_dynamic(ctx,90,1);
        if(nk_group_begin_titled(ctx, "group_unpack", "Unpack/Repack", NK_WINDOW_TITLE|NK_WINDOW_BORDER)){
            nk_layout_row_static(ctx, 0, 100, 2);
            if (nk_button_label(ctx, "Unpack")){
                std::string scriptFilename=OpenFilename("YGO LOTD LE Script Data Files (*.bin)\0*.*\0");
                if (!scriptFilename.empty()){
                    std::ifstream scriptFile(scriptFilename,std::ios::in|std::ios::binary);
                    if (scriptFile.is_open()){
                        Long pointerToPointers=ReadLong(scriptFile);
                        Int titleCount=ReadInt(scriptFile);
                        Int scriptBlockCount=ReadInt(scriptFile);
                        std::vector<Int> titleStartPos;
                        std::vector<Int> titleEndPos;
                        std::vector<Long> titlePointer;
                        for (Int i=0; i<titleCount;i++){
                            titleStartPos.push_back(ReadInt(scriptFile));
                            titleEndPos.push_back(ReadInt(scriptFile));
                            titlePointer.push_back(ReadLong(scriptFile));
                        }
                        std::vector<std::string> titleNames;
                        for (Int i=0; i<titleCount;i++){
                            scriptFile.seekg(titlePointer[i],scriptFile.beg);
                            titleNames.push_back(ReadByteString(scriptFile));
                        }

                        std::vector<std::vector<SCRIPT_BLOCK> > script;
                        for (Int i=0; i< titleCount; i++){
                            std::vector<SCRIPT_BLOCK> scriptBlocks;
                            Int blockCount=titleEndPos[i]-titleStartPos[i]+1;
                            int pointer;
                            for (Int j=0; j<blockCount;j++){
                                SCRIPT_BLOCK scriptBlock;
                                scriptFile.seekg(pointerToPointers+titleStartPos[i]*32+j*32);
                                pointer=ReadLong(scriptFile);
                                scriptFile.seekg(pointer,scriptFile.beg);
                                scriptBlock.chara=ReadByteString(scriptFile);
                                scriptFile.seekg(pointerToPointers+titleStartPos[i]*32+j*32+8);
                                pointer=ReadLong(scriptFile);
                                scriptFile.seekg(pointer,scriptFile.beg);
                                scriptBlock.arg1=ReadByteString(scriptFile);
                                scriptFile.seekg(pointerToPointers+titleStartPos[i]*32+j*32+16);
                                pointer=ReadLong(scriptFile);
                                scriptFile.seekg(pointer,scriptFile.beg);
                                scriptBlock.arg2=ReadByteString(scriptFile);
                                scriptFile.seekg(pointerToPointers+titleStartPos[i]*32+j*32+24);
                                pointer=ReadLong(scriptFile);
                                scriptFile.seekg(pointer,scriptFile.beg);
                                scriptBlock.text=ReadByteString(scriptFile);
                                scriptBlocks.push_back(scriptBlock);
                            }
                            script.push_back(scriptBlocks);
                        }
                        scriptFile.close();
                        if (!startingPath.empty()){
                            std::filesystem::create_directory(startingPath+"\\unpacked\\");
                            if (std::filesystem::is_directory(startingPath+"\\unpacked\\")){
                                for (Int i=0; i<titleCount;i++){
                                    std::ofstream outFile(startingPath+"\\unpacked\\"+titleNames[i]);
                                    if (outFile.is_open()){
                                        for (std::size_t j=0; j< script[i].size();j++){
                                            outFile << "char -> " << script[i][j].chara << "\n";
                                            outFile << "arg1 -> " << script[i][j].arg1 << "\n";
                                            outFile << "arg2 -> " << script[i][j].arg2 << "\n";
                                            outFile << "text -> " << script[i][j].text;
                                            if (j+1<script[i].size())
                                                outFile << "\n";
                                        }
                                        outFile.close();
                                    }
                                }
                            }else{
                                infoString="Could not create directory ./unpacked/";
                            }
                            
                            infoString="Unpacked " + std::to_string(titleCount) +" scripts to " + startingPath+"\\unpacked\\";
                        }else{
                            infoString="Could not retrieve directory to unpack to";
                        }
                    }else{
                        infoString="Could not open file " + scriptFilename;
                    }
                }else{
                    infoString="Canceled file dialog";
                }
                
            }
            if (nk_button_label(ctx, "Repack")){
                if (std::filesystem::is_directory(startingPath+"\\unpacked\\")){
                    std::vector<std::vector<SCRIPT_BLOCK> > script;
                    std::vector<std::string> titleNames;
                    int totalTitleNameLengths=0;
                    int totalBlockCount=0;
                    int titleCount=0;
                    std::vector<std::string> missingStories;
                    for (const auto& entry : std::filesystem::directory_iterator(startingPath+"\\unpacked\\")) {
                        std::string filenameStr = entry.path().string();
                        if (entry.is_regular_file()){
                            std::ifstream storyFile(filenameStr);
                            if (storyFile.is_open()){
                                std::string line;
                                std::vector<std::string> lines;
                                while (std::getline(storyFile,line)){
                                    if (line.length()>=7){
                                        lines.push_back(line);
                                    }
                                }
                                if (lines.size()%4==0){
                                    int blockCount=lines.size()/4;
                                    std::vector<SCRIPT_BLOCK> scriptBlocks;
                                    for (int i=0;i<blockCount;i++){
                                        SCRIPT_BLOCK scriptBlock;
                                        scriptBlock.chara=SimplifyString(lines[i*4].substr(7));
                                        scriptBlock.arg1=SimplifyString(lines[i*4+1].substr(7));
                                        scriptBlock.arg2=SimplifyString(lines[i*4+2].substr(7));
                                        scriptBlock.text=SimplifyString(lines[i*4+3].substr(7));
                                        scriptBlocks.push_back(scriptBlock);
                                    }
                                    script.push_back(scriptBlocks);
                                    totalBlockCount+=blockCount;
                                    titleNames.push_back(entry.path().filename().string());
                                    titleCount++;
                                    totalTitleNameLengths+=titleNames.back().length()+1;
                                }else{
                                    missingStories.push_back(filenameStr);
                                }
                                storyFile.close();
                            }else{
                                missingStories.push_back(filenameStr);
                            }
                        }
                    }
                    std::string outFilename=SaveFilename("YGO LOTD LE Script Data Files (*.bin)\0*.*\0");
                    if (!outFilename.empty()){
                        std::ofstream outFile(outFilename,std::ios::out|std::ios::binary);
                        if (outFile.is_open()){
                            WriteLong(outFile,16+titleCount*16+totalTitleNameLengths);
                            WriteInt(outFile,titleCount);
                            WriteInt(outFile,totalBlockCount);
                            Long titleOffset=16+titleCount*16;
                            Int blockCountAcc=0;
                            for (int i=0; i<titleCount;i++){
                                WriteInt(outFile,blockCountAcc);
                                blockCountAcc+=script[i].size();
                                WriteInt(outFile,blockCountAcc-1);
                                WriteLong(outFile,titleOffset);
                                titleOffset+=titleNames[i].length()+1;
                            }
                            for (int i=0; i<titleCount;i++){
                                WriteByteString(outFile,titleNames[i]);
                            }
                            Long textOffset=16+titleCount*16+totalTitleNameLengths+totalBlockCount*4*8;
                            for (int i=0; i<titleCount;i++){
                                for (int j=0; j<script[i].size();j++){
                                    WriteLong(outFile,textOffset);
                                    textOffset+=script[i][j].chara.size()+1;
                                    WriteLong(outFile,textOffset);
                                    textOffset+=script[i][j].arg1.size()+1;
                                    WriteLong(outFile,textOffset);
                                    textOffset+=script[i][j].arg2.size()+1;
                                    WriteLong(outFile,textOffset);
                                    textOffset+=script[i][j].text.size()+1;
                                }
                            }
                            for (int i=0; i<titleCount;i++){
                                for (int j=0; j<script[i].size();j++){
                                    WriteByteString(outFile,script[i][j].chara);
                                    WriteByteString(outFile,script[i][j].arg1);
                                    WriteByteString(outFile,script[i][j].arg2);
                                    WriteByteString(outFile,script[i][j].text);
                                }
                            }
                            outFile.close();
                            if (missingStories.empty()){
                                infoString="Successfully repacked " + std::to_string(titleCount) + " stories into " + outFilename;
                            }else{
                                std::ofstream missingFile(startingPath+"\\missingStories.txt");
                                if (missingFile.is_open()){
                                    for (std::size_t i=0; i<missingStories.size();i++){
                                        missingFile<<missingStories[i]<<"\n";
                                    }
                                    missingFile.close();
                                    infoString="Warning: Repacked some stories from "+ startingPath+"\\unpacked\\, but some were omitted (check missingStories.txt)";
                                }else{
                                    infoString="Warning: Only repacked some stories from "+ startingPath+"\\unpacked\\, not all";
                                }
                                
                            }
                        }else{
                            infoString="Could not create file "+outFilename;
                        }
                    }else{
                        infoString="Canceled file dialog";
                    }
                }else{
                    infoString="Did not find ./unpacked/ directory. Unpack scripts first!";
                }
                
            }
            nk_group_end(ctx);
        }
        
        nk_layout_row_dynamic(ctx,0,1);
        nk_edit_string_zero_terminated(ctx,NK_EDIT_INACTIVE,const_cast<char*>(("INFO: "+infoString).c_str()),INT_MAX,nk_filter_default);
    }
    nk_end(ctx);
}


void RenderGui(){
    nk_gdi_render(nk_rgb(30,30,30));
}

void CleanupGui(){
    nk_gdifont_del(font);
}

void UpdateWindowSize(unsigned int width, unsigned int height){
    windowWidth=width;
    windowHeight=height;
}