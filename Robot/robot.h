/*
 * robot.h
 *
 *  Created on: Aug 27, 2026
 *      Author: Kelvin Novais
 */

#ifndef ROBOT_H_
#define ROBOT_H_

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

EXTERN_C void setup(void);
EXTERN_C void loop(void);

#undef EXTERN_C

#endif /* ROBOT_H_ */
