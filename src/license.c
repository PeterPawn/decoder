/*
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2017, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
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
 * along with this program, please look for the file LICENSE.
 */

#define	LICENSE_C

#include "common.h"

// display license text

static	char *	license = "\
Copyright (C) 2014-2017 P.Haemmerlein (peterpawn@yourfritz.de)\n\n\
This project is free software, you can redistribute it and/or modify it under the terms of the GNU\n\
General Public License as published by the Free Software Foundation; either version 2 of the\n\
License, or (at your option) any later version.\n\n\
This project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without\n\
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
See the GNU General Public License under http://www.gnu.org/licenses/gpl-2.0.html for more details.\n\
";

void 	showLicense(FILE * out)
{
	fprintf(out, license);
}
