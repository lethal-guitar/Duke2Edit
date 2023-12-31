/** Copyright 2023 Nikolai Wuttke-Hohendorf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#ifndef CONSTANTS_H
#define CONSTANTS_H

enum ACTORTYPE
{
  AT_ALL,
  AT_BOSS,
  AT_BADGUY,
  AT_WEAPON,
  AT_GOODIE,
  AT_PLAYER,
  AT_SPECIALITEM,
  AT_ENVIRONMENT,
  AT_INTERACTIVE,
  AT_TECHNO,
  AT_TRIGGER,
  AT_MISC
};

struct ACTORNAMEINFO
{
  char Name[128];
  ACTORTYPE Type;
  char ImageId[64];
};

const int TILESIZE_GAME = 8;
const int TILESIZE_EDITOR = 16;

const char ActorTypes[][16] = {
  "<all>",
  "Bosses",
  "Badguys",
  "Weapons",
  "Goodies",
  "Player related",
  "Special items",
  "Environment",
  "Interactive",
  "Techno",
  "Triggers",
  "Misc",
  "\0"};


// The ImageId member is a string describing a list of one or more sprite
// images to display for a specific actor. The entries in the list are
// separated by semicolons. Each entry consists of three comma-separated
// numbers. The numbers are: Sprite ID, X offset, Y offset. Both offsets are in
// tiles. The sprite ID is _not_ the same as the actor ID used to index the
// ACTRINFO.MNI file. The latter has a hierarchical system, where an actor ID
// refers to a "sprite" with one or more animation frames.
//
// The ID used here on the other hand is an index into the flattened list of
// all sprite animation frames.
const ACTORNAMEINFO ActorDefs[] = {
  {"Robot drone", AT_BADGUY, "0, 0, 0; 6, 0, 1;"}, // MULTIIMG
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Duke, <-", AT_PLAYER, "37, 0, 0;"},
  {"Duke, ->", AT_PLAYER, "76, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Nuclear waste can, empty", AT_ENVIRONMENT, "124, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Green box - rocket launcher", AT_WEAPON, "130, 0, 0"},
  {"Green box - flame thrower", AT_WEAPON, "131, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Green box - normal weapon", AT_WEAPON, "133, 0, 0"},
  {"Green box - laser", AT_WEAPON, "134, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Blue box - health molecule", AT_GOODIE, "139, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Fast green cat, <-", AT_BADGUY, "216, 0, 0"},
  {"Fast green cat, ->", AT_BADGUY, "219, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"White box - circuit card", AT_SPECIALITEM, "225, 0, 0"},
  {"Flamethrower ->", AT_BADGUY, "226, 0, 0"},
  {"Flamethrower <-", AT_BADGUY, "227, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Red box - bomb", AT_BADGUY, "243, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Blue bonus globe 1", AT_GOODIE, "259, 0, 0; 260, 0, 0;"}, // MULTIIMG
  {"Blue bonus globe 2", AT_GOODIE, "259, 0, 0; 264, 0, 0;"}, // MULTIIMG
  {"Blue bonus globe 3", AT_GOODIE, "259, 0, 0; 268, 0, 0;"}, // MULTIIMG
  {"Blue bonus globe 4", AT_GOODIE, "259, 0, 0; 272, 0, 0;"}, // MULTIIMG
  {"Bouncing sentry robot", AT_BADGUY, "277, 0, 0"},
  {"Teleport 1", AT_INTERACTIVE, "285, 0, 0"},
  {"Teleport 2", AT_INTERACTIVE, "286, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"White box - rapid fire", AT_SPECIALITEM, "289, 0, 0"},
  {"Rocket launcher turret", AT_BADGUY, "296, 0, 0"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Bouncing sentry robot container",
   AT_BADGUY,
   "308, 0, 0; 306, 0, 1;"}, // MULTIIMG
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Bomb dropping spaceship", AT_BADGUY, "316, 0, 0; 319, 2, 3;"}, // MULTIIMG
  {"??", AT_MISC, "0, 0, 0;"},
  {"Bouncing spike ball", AT_BADGUY, "322, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Electric reactor", AT_TECHNO, "334, 0, 0;"},
  {"Green slime ball", AT_BADGUY, "338, 0, 0;"},
  {"Green slime container", AT_BADGUY, "356, 0, 0; 350, 0, 3;"}, // MULTIIMG
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Nuclear waste can, green slime inside", AT_ENVIRONMENT, "124, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Snake", AT_BADGUY, "400, 0, 0;"},
  {"Camera - on ceiling", AT_TECHNO, "418, 0, 0;"},
  {"Camera - on floor", AT_TECHNO, "430, 0, 0;"},
  {"Green hanging suction plant", AT_BADGUY, "434, 0, 0;"},
  {"Appear only in med/hard difficulty", AT_TRIGGER, "-"},
  {"Appear only in hard difficulty", AT_TRIGGER, "-"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Duke's ship", AT_PLAYER, "451, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Force shield (need cloaking device)", AT_TECHNO, "466, 0, 0"}, // MULTIIMG
  {"??", AT_MISC, "0, 0, 0;"},
  {"Rocket 1 (falls over and explodes)", AT_TECHNO, "476, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Cross walker", AT_BADGUY, "489, 0, 0;"},
  {"Eyeball bomber plant", AT_BADGUY, "497, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"BOSS Episode 2", AT_BOSS, "513, 0, 0;"},
  {"Explosive charge 1 (falls down and sinks into ground)", AT_MISC, "-"},
  {"Explosive charge marker 1", AT_MISC, "-"},
  {"Explosive charge marker 2", AT_MISC, "-"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Explosive charge 2 (explodes into small parts flying away)", AT_MISC, "-"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"White box - cloaking device", AT_SPECIALITEM, "529, 0, 0;"},
  {"Sentry robot generator", AT_BADGUY, "533, 1, 0; 537, 0, 3;"}, // MULTIIMG
  {"Explosive charge 3", AT_MISC, "-"},
  {"Pipe dripping green stuff", AT_ENVIRONMENT, "538, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Circuit card door", AT_TECHNO, "543, 0, 0;"}, // MULTIIMG
  {"Circuit card keyhole", AT_INTERACTIVE, "548, 0, 0;"},
  {"White box blue key", AT_SPECIALITEM, "552, 0, 0;"},
  {"Blue key keyhole", AT_INTERACTIVE, "553, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Auto-open vertical door",
   AT_INTERACTIVE,
   "604, 0, 1; 605, 0, 2; 606, 0, 3; 607, 0, 4; 599, 0, 5;"}, // MULTIIMG
  {"Keyhole mounting pole", AT_TECHNO, "608, 0, 0;"},
  {"Vertical fan", AT_INTERACTIVE, "985, 0, 0; 609, 0, 1;"},
  {"Swivel gun", AT_BADGUY, "613, 0, 0;"},
  {"Auto-open horizontal door", AT_INTERACTIVE, "621, 0, 0;"},
  {"Restart beacon", AT_INTERACTIVE, "624, 0, 0;"},
  {"Skeleton", AT_BADGUY, "634, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Explosive charge 4", AT_MISC, "-"},
  {"Explosive charge 5 (falls down but remains intact)", AT_MISC, "-"},
  {"Exit", AT_TRIGGER, "-"},
  {"Swivel gun mounting post", AT_TECHNO, "651, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Explosive charge 6", AT_MISC, "-"},
  {"Explosive charge 7", AT_MISC, "-"},
  {"Rocket 2", AT_TECHNO, "643, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Metal crunch jaws", AT_BADGUY, "652, 0, 0;"},
  {"Floating split laser ball", AT_BADGUY, "655, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Spider", AT_BADGUY, "669, 0, 0;"},
  {"Blue box - N", AT_GOODIE, "679, 0, 0;"},
  {"Blue box - U", AT_GOODIE, "680, 0, 0;"},
  {"Blue box - K", AT_GOODIE, "681, 0, 0;"},
  {"Blue box - E", AT_GOODIE, "682, 0, 0;"},
  {"Blue guard ->", AT_BADGUY, "683, 0, 0;"},
  {"Blue box - video game cartridge", AT_GOODIE, "699, 0, 0;"},
  {"White box - empty", AT_MISC, "700, 0, 0;"},
  {"Green box - empty", AT_MISC, "701, 0, 0;"},
  {"Red box - empty", AT_MISC, "702, 0, 0;"},
  {"Blue box - empty", AT_MISC, "703, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Red box - cola", AT_GOODIE, "716, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Blue guard <-", AT_BADGUY, "689, 0, 0;"},
  {"Blue box - sunglasses", AT_GOODIE, "726, 0, 0;"},
  {"Blue box - phone", AT_GOODIE, "727, 0, 0;"},
  {"Red box - 6 pack cola", AT_GOODIE, "728, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Ugly green bird", AT_BADGUY, "730, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Blue box - boom box", AT_GOODIE, "740, 0, 0;"},
  {"Blue box - disk", AT_GOODIE, "741, 0, 0;"},
  {"Blue box - TV", AT_GOODIE, "742, 0, 0;"},
  {"Blue box - camera", AT_GOODIE, "743, 0, 0;"},
  {"Blue box - PC", AT_GOODIE, "744, 0, 0;"},
  {"Blue box - CD", AT_GOODIE, "746, 0, 0;"},
  {"Blue box - M", AT_GOODIE, "747, 0, 0;"},
  {"Rotating floor spikes", AT_BADGUY, "748, 0, 0;"},
  {"Stone statue <-", AT_BADGUY, "752, 0, 0;"},
  {"Stone statue ->", AT_BADGUY, "758, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"BOSS Episode 1", AT_BOSS, "776, 0, 2; 778, 2, 0;"}, // MULTIIMG
  {"Red box - turkey", AT_GOODIE, "780, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Small red bird", AT_BADGUY, "789, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Floating exit sign ->", AT_PLAYER, "800, 0, 0;"},
  {"Rocket elevator", AT_INTERACTIVE, "807, 0, 0; 802, 0, 3;"}, // MULTIIMG
  {"Message box 1", AT_TECHNO, "810, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Lava surface", AT_ENVIRONMENT, "812, 0, 0;"},
  {"Flying message ship \"YOUR BRAIN IS OURS\"",
   AT_BADGUY,
   "519, 0, 0; 520, 6, 0; 522, 1, 4; 816, 1, 1;"}, // MULTIIMG
  {"Flying message ship \"BRING BACK THE BRAIN\"",
   AT_BADGUY,
   "519, 0, 0; 520, 6, 0; 522, 1, 4; 820, 1, 1;"}, // MULTIIMG
  {"Flying message ship \"LIVE FROM RIGEL IT'S SATURDAY NIGHT\"",
   AT_BADGUY,
   "519, 0, 0; 520, 6, 0; 522, 1, 4; 830, 1, 1;"}, // MULTIIMG
  {"Flying message ship \"DIE\"",
   AT_BADGUY,
   "519, 0, 0; 520, 6, 0; 522, 1, 4; 842, 1, 1;"}, // MULTIIMG
  {"Blue guard (using a terminal)", AT_BADGUY, "695, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Smasher", AT_BADGUY, "844, 1, 0; 843, 0, 1;"},
  {"Flying message ship \"YOU CANNOT ESCAPE US YOU WILL GET YOUR BRAIN SUCKED\"",
   AT_BADGUY,
   "519, 0, 0; 520, 6, 0; 522, 1, 4; 845, 1, 1;"}, // MULTIIMG
  {"Water depths", AT_ENVIRONMENT, "-"},
  {"Lava fall 1", AT_ENVIRONMENT, "852, 0, 0;"},
  {"Lava fall 2", AT_ENVIRONMENT, "856, 0, 0;"},
  {"Water fall 1", AT_ENVIRONMENT, "860, 0, 0;"},
  {"Water fall 2", AT_ENVIRONMENT, "864, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Water drip", AT_ENVIRONMENT, "868, 0, 0;"},
  {"Water fall splash left", AT_ENVIRONMENT, "869, 0, 0;"},
  {"Water fall splash center", AT_ENVIRONMENT, "872, 0, 0;"},
  {"Water fall splash right", AT_ENVIRONMENT, "875, 0, 0;"},
  {"Lava riser", AT_ENVIRONMENT, "881, 0, 0; 878, 0, 1;"}, // MULTIIMG
  {"??", AT_MISC, "0, 0, 0;"},
  {"Water surface 1", AT_ENVIRONMENT, "-"},
  {"Water surface 2", AT_ENVIRONMENT, "-"},
  {"Green slime liquid", AT_ENVIRONMENT, "885, 0, 0;"},
  {"Radar antenna", AT_TECHNO, "889, 0, 0;"},
  {"Message box 2",
   AT_TECHNO,
   "901, 4, 3; 902, 0, 4; 903, 5, 7; 904, 6, 0; 905, 4, 3"}, // MULTIIMG
  {"??", AT_MISC, "0, 0, 0;"},
  {"Special hint globe", AT_SPECIALITEM, "922, 0, 0;"},
  {"Special hint machine", AT_INTERACTIVE, "925, 0, 0;"},
  {"Generates windblown spiders when touched", AT_TRIGGER, "-"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Small unicycle", AT_BADGUY, "934, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Flame jet 1", AT_ENVIRONMENT, "941, 0, 0;"},
  {"Flame jet 2", AT_ENVIRONMENT, "945, 0, 0;"},
  {"Flame jet 3", AT_ENVIRONMENT, "949, 0, 0;"},
  {"Flame jet 4", AT_ENVIRONMENT, "953, 0, 0;"},
  {"Airlock effect <-", AT_TRIGGER, "-"},
  {"Airlock effect ->", AT_TRIGGER, "-"},
  {"Floating exit sign <-", AT_PLAYER, "957, 0, 0;"},
  {"Caged claw monster, active", AT_BADGUY, "959, 0, 0;"},
  {"Generates explosion when seen", AT_TRIGGER, "-"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Water on floor 1", AT_ENVIRONMENT, "974, 0, 0;"},
  {"Water on floor 2", AT_ENVIRONMENT, "978, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Caged claw monster, inactive", AT_ENVIRONMENT, "991, 0, 0;"},
  {"Fire on floor 1", AT_ENVIRONMENT, "993, 0, 0;"},
  {"Fire on floor 2", AT_ENVIRONMENT, "997, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"BOSS Episode 3", AT_BOSS, "1002, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Small flying ship 1", AT_BADGUY, "1010, 0, 0;"},
  {"Small flying ship 2", AT_BADGUY, "1013, 0, 0;"},
  {"Small flying ship 3", AT_BADGUY, "1016, 0, 0;"},
  {"Blue box - T shirt", AT_GOODIE, "1019, 0, 0;"},
  {"Blue box - videocassette", AT_GOODIE, "1020, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"BOSS Episode 4", AT_BOSS, "1032, 2, 0; 1034, 0, 2"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Floating arrow", AT_PLAYER, "1059, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"Rigelatin soldier", AT_BADGUY, "1066, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  {"??", AT_MISC, "0, 0, 0;"},
  //{ "Blue box - video", AT_GOODIE, "0, 0, 0;" },
  {"\0", AT_MISC, "0, 0, 0;"},
};

/*
const char ActorDescriptions[][512] =
{
        "Fast robot drone. Teleports in when it gets into view. It chases Duke
until it gets destroyed.", "Marks the starting point for the player. Facing
right initially.", "Marks the starting point for the player. Facing left
initially.", "Will explode when shot. Causes no harm.", "Reveals a rocket
launcher when shot.", "Reveals a flame thrower when shot.", "Reveals a normal
weapon when shot.", "Reveals a laser when shot.", "Reveals a health molecule
when shot. Health molecules restore 1 unit of health. If the player is at full
health, they earn him much more points than usually.", "Fast green creature
chasing Duke. Facing right initially.", "Fast green creature chasing Duke.
Facing left initially.", "Reveals a circuit card when shot. The card can be used
to disable force fields.", "A wall mounted auto-shooting flamethrower, moving up
and down. Facing right.", "A wall mounted auto-shooting flamethrower, moving up
and down. Facing left.", "Reveals a bomb when shot. The bomb will explode after
a short period of time, causing a deadly ring of fire on the ground.", "This
earns you x points.", "This earns you points.", "This earns you points.", "This
earns you points.", "Robot drone with enhanced visual sensors. It will bounce
around trying to reach the player.", "This allows Duke to travel to another part
of the level. This is a teleport starting point. It will teleport Duke to the
next teleport destination in reach.", "This allows Duke to travel to another
part of the level. This is a teleport destination.", "Reveals a rapid fire
powerup when shot. Rapid fire allows the player to hold down the fire button
instead of having to press for every single shot.", "A small ground mounted
auto-aiming rocket-launcher.", "This transports a bouncing sentry robot.", "This
spacecraft flys over the players head and drops a very dangerous napalm bomb on
him.", "This bounces up and down until it gets damaged, which causes it to
bounce wildly in every direction.", "This high energy reactor blocks the players
way. Contact will result in sudden death. It can be destroyed by normal weapon
fire, anyway.", "A green slime creature which is able to stick itself to the
ceiling and then drop down on the player.", "A glass container containing a
green slime ball. If it's shot, the slime ball will be released.", "This behaves
in the same way as the empty nuclear waste can, except that it causes small
radius damage if it's shot.", "A large snake, able to swallow the player. If
that happens, Duke can free himself by killing the snake.", "A security camera.
Destroy all of them to get secret bonus 1. Ceiling mounted.", "A security
camera. Destroy all of them to get secret bonus 1. Floor mounted.", "This
creature is also able to swallow Duke, but he will be released after a period of
time. Nevertheless, being swallowed causes a lot of damage.", "This causes the
object to the right to appear only in med/hard difficulty.", "This causes the
object to the right to appear only in hard difficulty.", "Duke's ship",
*/

#endif