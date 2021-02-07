#include "main.h"
#include "my_filter.h"
#include "iir_filter.h"

//fmax = 0.25 Т.е. фильтр в 4 раза частоту уменьшает максимальную
//signal.cheby2(16, 70, fmax, 'low', analog=False, output="sos")
#define coeff_sos_x4_sections 8
const float coeff_sos_x4[]={
    0.001282070078055012, 0.002292205228482419, 0.001282070078055012, 1.0, -0.34166847751339063, 0.03821970324808857,
    1.0, 0.6825229858195228, 1.0000000000000002, 1.0, -0.4664894834231138, 0.12529535331799338,
    1.0, -0.25720458349397307, 0.9999999999999998, 1.0, -0.6659807096858221, 0.26591023949407566,
    1.0, -0.8044279326878758, 1.0, 1.0, -0.8820990647916817, 0.4214917656960164,
    1.0, -1.1076902781388334, 1.0, 1.0, -1.0792584857522882, 0.5689644539094694,
    1.0, -1.2770993587568327, 0.9999999999999998, 1.0, -1.2452794241061382, 0.7015680454138651,
    1.0, -1.368815733805565, 1.0, 1.0, -1.381216713379076, 0.8223360479588602,
    1.0, -1.4093701647021322, 1.0, 1.0, -1.4933941303028764, 0.9393521857339246,
    };

float zi_0[coeff_sos_x4_sections*2];
FilterData data0 = {
    .n_sections = coeff_sos_x4_sections,
    .coeff = coeff_sos_x4,
    .zi = zi_0
};

float zi_1[coeff_sos_x4_sections*2];
FilterData data1 = {
    .n_sections = coeff_sos_x4_sections,
    .coeff = coeff_sos_x4,
    .zi = zi_0
};

void MyFilterInit()
{
    SosFilterInit(&data0);
    SosFilterInit(&data1);
}

float SosFilterProcess_x4_0(float newX)
{
    return SosFilterProcess(&data0, newX);
}


float SosFilterProcess_x4_1(float newX)
{
    return SosFilterProcess(&data1, newX);
}
