#pragma once

#include <array>
#include "Color.h"

namespace common {
    constexpr std::array<Color, 51> ColorTable {
            Color{ 0, 0, 0, 0 },			 /* CT_NONE */
            Color{ 0, 0, 0, 1 },			 /* CT_BLACK */
            Color{ 1, 0, 0, 1 },			 /* CT_RED */
            Color{ 0, 1, 0, 1 },			 /* CT_GREEN */
            Color{ 0, 0, 1, 1 },			 /* CT_BLUE */
            Color{ 1, 1, 0, 1 },			 /* CT_YELLOW */
            Color{ 1, 0, 1, 1 },			 /* CT_MAGENTA */
            Color{ 0, 1, 1, 1 },			 /* CT_CYAN */
            Color{ 0.071f, 0.271f, 0.29f, 1 }, /* CT_TEAL */
            Color{ 0.529f, 0.373f, 0.017f, 1 },/* CT_GOLD */
            Color{ 1, 1, 1, 1 },			 /* CT_WHITE */
            Color{ 0.75f, 0.75f, 0.75f, 1 },	 /* CT_LTGREY */
            Color{ 0.50f, 0.50f, 0.50f, 1 },	 /* CT_MDGREY */
            Color{ 0.25f, 0.25f, 0.25f, 1 },	 /* CT_DKGREY */
            Color{ 0.15f, 0.15f, 0.15f, 1 },	 /* CT_DKGREY2 */

            Color{ 0.688f, 0.797f, 1, 1 },	 /* CT_VLTORANGE -- needs values */
            Color{ 0.688f, 0.797f, 1, 1 },	 /* CT_LTORANGE */
            Color{ 0.620f, 0.710f, 0.894f, 1 },/* CT_DKORANGE */
            Color{ 0.463f, 0.525f, 0.671f, 1 },/* CT_VDKORANGE */

            Color{ 0.616f, 0.718f, 0.898f, 1 },/* CT_VLTBLUE1 */
            Color{ 0.286f, 0.506f, 0.898f, 1 },/* CT_LTBLUE1 */
            Color{ 0.082f, 0.388f, 0.898f, 1 },/* CT_DKBLUE1 */
            Color{ 0.063f, 0.278f, 0.514f, 1 },/* CT_VDKBLUE1 */

            Color{ 0.302f, 0.380f, 0.612f, 1 },/* CT_VLTBLUE2 -- needs values */
            Color{ 0.196f, 0.314f, 0.612f, 1 },/* CT_LTBLUE2 */
            Color{ 0.060f, 0.227f, 0.611f, 1 },/* CT_DKBLUE2 */
            Color{ 0.043f, 0.161f, 0.459f, 1 },/* CT_VDKBLUE2 */

            Color{ 0.082f, 0.388f, 0.898f, 1 },/* CT_VLTBROWN1 -- needs values */
            Color{ 0.082f, 0.388f, 0.898f, 1 },/* CT_LTBROWN1 */
            Color{ 0.078f, 0.320f, 0.813f, 1 },/* CT_DKBROWN1 */
            Color{ 0.060f, 0.227f, 0.611f, 1 },/* CT_VDKBROWN1 */

            Color{ 1, 0.784f, 0.365f, 1 },	 /* CT_VLTGOLD1 -- needs values */
            Color{ 1, 0.706f, 0.153f, 1 },	 /* CT_LTGOLD1 */
            Color{ 0.733f, 0.514f, 0.086f, 1 },/* CT_DKGOLD1 */
            Color{ 0.549f, 0.384f, 0.063f, 1 },/* CT_VDKGOLD1 */

            Color{ 0.688f, 0.797f, 1, 1 },	 /* CT_VLTPURPLE1 -- needs values */
            Color{ 0.688f, 0.797f, 1, 1 },	 /* CT_LTPURPLE1 */
            Color{ 0.313f, 0.578f, 1, 1 },	 /* CT_DKPURPLE1 */
            Color{ 0.031f, 0.110f, 0.341f, 1 },/* CT_VDKPURPLE1 */

            Color{ 0.688f, 0.797f, 1, 1 },	 /* CT_VLTPURPLE2 -- needs values */
            Color{ 0.688f, 0.797f, 1, 1 },	 /* CT_LTPURPLE2 */
            Color{ 0.688f, 0.797f, 1, 1 },	 /* CT_DKPURPLE2 */
            Color{ 0.031f, 0.110f, 0.341f, 1 },/* CT_VDKPURPLE2 */

            Color{ 0.686f, 0.808f, 0.1f, 1 },	 /* CT_VLTPURPLE3 */
            Color{ 0.188f, 0.494f, 1, 1 },	 /* CT_LTPURPLE3 */
            Color{ 0.094f, 0.471f, 1, 1 },	 /* CT_DKPURPLE3 */
            Color{ 0.067f, 0.325f, 0.749f, 1 },/* CT_VDKPURPLE3 */

            Color{ 1, 0.612f, 0.325f, 1 },	 /* CT_VLTRED1 */
            Color{ 1, 0.478f, 0.098f, 1 },	 /* CT_LTRED1 */
            Color{ 1, 0.438f, 0, 1 },		 /* CT_DKRED1 */
            Color{ 0.784f, 0.329f, 0, 1 },	 /* CT_VDKRED1 */
    };
}
