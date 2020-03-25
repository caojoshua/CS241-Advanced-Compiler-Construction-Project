/*
 * SSAutils.h
 * Author: Joshua Cao
 */

#ifndef INCLUDE_SSA_SSAUTILS_H_
#define INCLUDE_SSA_SSAUTILS_H_

#include <string>

namespace SSA
{

enum Opcode : uint;

std::string opToStr(Opcode op);

}
#endif /* INCLUDE_SSA_SSAUTILS_H_ */
