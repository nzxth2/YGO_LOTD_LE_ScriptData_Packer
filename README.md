# YGO LOTD Link Evolution ScriptData Packer
 Unpacker/Repacker for .scriptdata_X.bin files of Yu-Gi-Oh! Legacy of the Duelist Link Evolution 
 
## Features
 - Unpacks any scriptdata_X.bin file to a directory (all languages should work, untested)
 - Repacks story files from that folder back into a .bin file
 - Allows for modification of the existing story or addition of new stories
  
 ### Script File Format
 ```
 char -> yugimuto
 arg1 -> LEFT
 arg2 -> smile
 text -> Sample dialog
 ```
  
 All extracted script files follow the same format. The filename is always composed of the duel name id + `_INTRO`/`_OUTRO`.  
 The contents are always divided in blocks of 4 lines as seen above. The first row of the block (`char`) is for characters, the second row (`arg1`) is related to character positioning, the third (`arg2`) to emotion and the fourth row (`text`) finally is for the dialog text. You don't change the left side of the block, that is used for formatting. You only change the right side.  
 `char` and `arg2` together decide which character image is used from the /pdui/dialog_chars/ folder. So obviously you can only choose combinations that match an image that is available there.  
 `arg1` can be any of the following:  
 ```
 BACK_LEFT
 LEFT
 LEFT:FADEIN
 LEFT:FRONT
 LEFT_BACK
 LEFT_FRONT
 Left
 BACK_RIGHT
 RIGHT
 RIGHT:FADEIN
 RIGHT:FADEOUT
 RIGHT_BACK
 RIGHT_FRONT
 Right
 CENTER
 CENTER:FADEIN
 FADEIN:CENTER
 FADEIN:LEFT
 FADEIN:RIGHT
 FADEOUT
 NONE
 None

 ```
 And for `text` there isn't anything special to note, except that the unpacker converts any *forced* linebreak to a `'|'` character and the repacker converts those back to forced linebreaks.  
 There is an exception to all of this, you can put `command` into the `char` row instead of a character name ID. This then changes the meaning of `arg1` and `arg2`: `arg1` can be `BG` or `PROP_ON`/`PROP_OFF` and `arg2` can be a background image from \pdui\dialog_bg\ or a prop image from \pdui\dialog_props\, respectively (you do not write the file extensions).
 
## Building
  Not crossplatform at all, only builds for Windows  
  Included the makefile used with gcc, a simple `make` should do the trick  
  No external libraries should be required

## Credits
 Developed by nzxth2  
 [Nuklear GUI library](https://github.com/Immediate-Mode-UI/Nuklear) developed by Micha Mettke and others  
 [Notes](https://github.com/MoonlitDeath/Legacy-of-the-Duelist-notes/wiki) and [guide](https://github.com/MoonlitDeath/Link-Evolution-Editing-Guide/wiki) by MoonlitDeath

## Gallery
 ![Screenshot](https://i.imgur.com/Nn8F3Vk.png)