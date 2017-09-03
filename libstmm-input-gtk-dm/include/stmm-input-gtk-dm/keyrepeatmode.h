/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   keyrepeatmode.h
 */

#ifndef STMI_KEY_REPEAT_MODE_H
#define STMI_KEY_REPEAT_MODE_H

namespace stmi
{

/** Key repeat translation type.
 * Key presses unanswered by a release are not allowed.
 *
 * For example the gdk event sequence for a specific key
 *
 *     Press  Press  Press  Release
 *
 * is translated in either
 *
 *     Press Release  Press Release  Press Release  (add release mode)
 *
 * or
 *
 *     Press Cancel   Press Cancel   Press Release  (add release cancel mode)
 *
 * or
 *
 *     Press                               Release  (suppress mode)
 *
 */
enum KEY_REPEAT_MODE {
	KEY_REPEAT_MODE_SUPPRESS = 1 /**< Suppress repeated key presses. */
	, KEY_REPEAT_MODE_ADD_RELEASE = 2 /**< Send a release to listeners prior a repeated key press. */
	, KEY_REPEAT_MODE_ADD_RELEASE_CANCEL = 3 /**< Send a release cancel to listeners prior a repeated key press. */
};

} // namespace stmi

#endif /* STMI_KEY_REPEAT_MODE_H */
