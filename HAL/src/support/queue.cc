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

#include <queue.hpp>
uint8_t Queue[QUEUE_SIZE];
uint8_t QueueIn, QueueOut;

void QueueInit(void)
{
  QueueIn = QueueOut = 0;
}

int QueuePut(char toadd)
{
  if (QueueIn == ((QueueOut - 1 + QUEUE_SIZE) % QUEUE_SIZE))
  {
    return -1; /* Queue Full*/
  }

  Queue[QueueIn] = toadd;

  QueueIn = (QueueIn + 1) % QUEUE_SIZE;

  return 0; // No errors
}

int QueueGet(char *old)
{
  if (QueueIn == QueueOut)
  {
    return -1; /* Queue Empty - nothing to get*/
  }

  *old = Queue[QueueOut];

  QueueOut = (QueueOut + 1) % QUEUE_SIZE;

  return 0; // No errors
}
