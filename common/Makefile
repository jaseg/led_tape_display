# Megumin LED display firmware
# Copyright (C) 2018 Sebastian Götte <code@jaseg.net>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

all: 8b10b_test_encode 8b10b_test_decode

8b10b_test_encode: 8b10b_test_encode.c 8b10b.c
	gcc -o $@ $^

8b10b_test_decode: 8b10b_test_decode.c 8b10b.c
	gcc -o $@ $^

clean:
	rm -f 8b10b_test_encode 8b10b_test_decode

