
#include "species.h"

Species::Species(float alt_r[2], float temp_r[2], float prec_r[2])
	: alt_range(), temp_range(), prec_range()
{
	alt_range[0] = alt_r[0];
	alt_range[1] = alt_r[1];
	temp_range[0] = temp_r[0];
	temp_range[1] = temp_r[1];
	prec_range[0] = prec_r[0];
	prec_range[1] = prec_r[1];
}