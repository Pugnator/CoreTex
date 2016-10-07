/*******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 2015
 *******************************************************************************/
#include <global.hpp>
#include <common.hpp>
#include <hal/io_macro.hpp>

#define MORSESPEED 20
#define DOTLEN (1200/MORSESPEED)

static const uint8_t morsetab[] =
  {
  /* 1 - no code */
  107, /* ASCII 33 ! */
  82, /* ASCII 34 " */
  1, /* ASCII 35 # */
  137, /* ASCII 36 $ */
  1, /* ASCII 37 % */
  40, /* ASCII 38 & */
  94, /* ASCII 39 ' */
  109, /* ASCII 40 ( */
  109, /* ASCII 41 ) */
  1, /* ASCII 42 * */
  42, /* ASCII 43 + */
  115, /* ASCII 44 , */
  97, /* ASCII 45 - */
  85, /* ASCII 46 . */
  50, /* ASCII 47 / */
  63, /* ASCII 48 0 */
  62, /* ASCII 49 1 */
  60, /* ASCII 50 2 */
  56, /* ASCII 51 3 */
  48, /* ASCII 52 4 */
  32, /* ASCII 53 5 */
  33, /* ASCII 54 6 */
  35, /* ASCII 55 7 */
  39, /* ASCII 56 8 */
  47, /* ASCII 57 9 */
  120, /* ASCII 58 : */
  53, /* ASCII 59 ; */
  1, /* ASCII 60 < */
  49, /* ASCII 61 = */
  1, /* ASCII 62 > */
  76, /* ASCII 63 ? */
  69, /* ASCII 64 @ */
  6, /* ASCII 65 A */
  17, /* ASCII 66 B */
  21, /* ASCII 67 C */
  9, /* ASCII 68 D */
  2, /* ASCII 69 E */
  20, /* ASCII 70 F */
  11, /* ASCII 71 G */
  16, /* ASCII 72 H */
  4, /* ASCII 73 I */
  30, /* ASCII 74 J */
  13, /* ASCII 75 K */
  18, /* ASCII 76 L */
  7, /* ASCII 77 M */
  5, /* ASCII 78 N */
  15, /* ASCII 79 O */
  22, /* ASCII 80 P */
  27, /* ASCII 81 Q */
  10, /* ASCII 82 R */
  8, /* ASCII 83 S */
  3, /* ASCII 84 T */
  12, /* ASCII 85 U */
  24, /* ASCII 86 V */
  14, /* ASCII 87 W */
  25, /* ASCII 88 X */
  29, /* ASCII 89 Y */
  19, /* ASCII 90 Z */
  1, /* ASCII 91 [ */
  1, /* ASCII 92 \ */
  1, /* ASCII 93 ] */
  1, /* ASCII 94 ^ */
  77, /* ASCII 95 _ */
  94, /* ASCII 96 ` */
  6, /* ASCII 97 a */
  17, /* ASCII 98 b */
  21, /* ASCII 99 c */
  9, /* ASCII 100 d */
  2, /* ASCII 101 e */
  20, /* ASCII 102 f */
  11, /* ASCII 103 g */
  16, /* ASCII 104 h */
  4, /* ASCII 105 i */
  30, /* ASCII 106 j */
  13, /* ASCII 107 k */
  18, /* ASCII 108 l */
  7, /* ASCII 109 m */
  5, /* ASCII 110 n */
  15, /* ASCII 111 o */
  22, /* ASCII 112 p */
  27, /* ASCII 113 q */
  10, /* ASCII 114 r */
  8, /* ASCII 115 s */
  3, /* ASCII 116 t */
  12, /* ASCII 117 u */
  24, /* ASCII 118 v */
  14, /* ASCII 119 w */
  25, /* ASCII 120 x */
  29, /* ASCII 121 y */
  19 /* ASCII 122 z */
  };

void
morse_dit (void)
{
  for (int i = 0; i < DOTLEN; i++)
    {
      PIN_TOGGLE(SWEEP);
      delay_ms (1);
    }
  delay_ms (500);
}

void
morse_dash ()
{
  for (int i = 0; i < 3 * DOTLEN; i++)
    {
      PIN_TOGGLE(SWEEP);
      delay_ms (1);
    }
  delay_ms ( DOTLEN);
}

void
morse_sendc (char c)
{
  uint8_t _i;
  uint8_t _p;

  // Send space
  if (c == ' ')
    {
      delay_ms (7 * DOTLEN);
      return;
    }
  // Do a table lookup to get morse data
  else
    {
      _i = ((uint8_t) c) - 33;
      _p = morsetab[_i];
    }

  // Main algoritm for each morse sign
  while (_p != 1)
    {
      if (_p & 1)
	morse_dash ();
      else
	morse_dit ();
      _p = _p / 2;
    }
  // Letterspace
  delay_ms (3 * DOTLEN);
}

void
morse_print (char* str)
{
  char* p = str;
  while (*p)
    {
      morse_sendc (*p++);
    }
}
