// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
//
// Netlist for Car Polo audio
//
// Derived from the schematics in the Car Polo manual.
//
// Known problems/issues:
//
//    * Uses HLE noise due to abusing a 2N3904 in breakdown as a noise source.
//    * Very slow
//

#include "netlist/devices/net_lib.h"

#define HLE_NOISE (1)

//
// Player Crash netlist
//

static NETLIST_START(PLAYER_CRASH)
	ALIAS(V15, R200.1)
	ALIAS(VCC, 4C.VCC)
	ALIAS(GND, R210.2)
	ALIAS(CRASH_IN, 4C.A)
	ALIAS(AUDIO_OUT, R217.1)
	ALIAS(NOISE_IN, CR10.K)

	TTL_7407_GATE(4C)
	NET_C(4C.GND, GND)

	RES(R200, RES_K(1.2))
	RES(R202, RES_K(150))
	RES(R203, RES_K(550))
	RES(R204, RES_K(100))
	RES(R205, RES_K(270))
	RES(R210, RES_K(33))
	RES(R212, RES_K(1))
	RES(R213, RES_K(10))
	RES(R216, RES_K(47))
	RES(R217, RES_K(5))

	CAP(C89, CAP_U(0.1))
	CAP(C90, CAP_U(0.01))
	CAP(C91, CAP_U(0.01))
	CAP(C92, CAP_U(6.8))
	CAP(C93, CAP_U(2.2))

	DIODE(CR10, "1N4454")
	DIODE(CR11, "1N4454")
	DIODE(CR14, "1N4454")

	QBJT_EB(Q6, "2N3904")

	NET_C(R200.1, R212.1, R216.1)
	NET_C(R200.2, 4C.Y, CR11.A)
	NET_C(CR11.K, C93.1, R204.1, R205.1)
	NET_C(C93.2, R205.2, Q6.E, R217.1, R210.2, C92.2)
	NET_C(R204.2, R203.1, R202.1, CR10.A)
	NET_C(R202.2, C89.1)
	NET_C(C89.2, C90.1, R203.2, Q6.B)
	NET_C(C90.2, R210.1, C91.1)
	NET_C(C91.2, Q6.C, CR14.K, R213.1)
	NET_C(R213.2, R212.2, C92.1)
	NET_C(R217.2, CR14.A, R216.2)
NETLIST_END()

//
// Main netlist
//

NETLIST_START(carpolo)

	SOLVER(Solver, 48000)
	ANALOG_INPUT(V15, 15)
	ANALOG_INPUT(V7_5, 7.5)
	ANALOG_INPUT(V5, 5)
	ALIAS(VCC, V5)

	TTL_INPUT(PL1_CRASH, 1) // active high
	TTL_INPUT(PL2_CRASH, 0) // active high
	TTL_INPUT(PL3_CRASH, 0) // active high
	TTL_INPUT(PL4_CRASH, 0) // active high

	LOCAL_SOURCE(PLAYER_CRASH)

	NET_C(GND, PL1_CRASH.GND, PL2_CRASH.GND, PL3_CRASH.GND, PL4_CRASH.GND)
	NET_C(VCC, PL1_CRASH.VCC, PL2_CRASH.VCC, PL3_CRASH.VCC, PL4_CRASH.VCC)

	// HLE Noise Gen
	CLOCK(NOISE_CLOCK, 10000)
	NET_C(NOISE_CLOCK.GND, GND)
	NET_C(NOISE_CLOCK.VCC, VCC)

	SYS_NOISE_MT_N(NOISE, 0.1)
	NET_C(NOISE.I, NOISE_CLOCK.Q)

	// White Noise
#if HLE_NOISE
	NET_C(NOISE.1, V7_5)
	ALIAS(WHITE_NOISE, NOISE.2)
#else
	RES(R240, RES_K(1.2))
	RES(R241, RES_M(1))
	RES(R242, 100)
	RES(R243, RES_M(1))
	RES(R244, RES_K(47))
	RES(R245, RES_K(10))
	RES(R246, RES_K(1))
	RES(R247, RES_K(10))
	RES(R248, RES_M(1))

	CAP(C99,  CAP_U(2.2))
	//CAP(C100, CAP_P(100))
	CAP(C101, CAP_U(6.8))
	CAP(C102, CAP_U(2.2))
	CAP(C103, CAP_U(6.8))

	//QBJT_EB(Q8, "2N3904") // Used as a reverse-biased noise source.
	QBJT_EB(Q9, "2N3904")

	OPAMP(1A, "LM748")
	NET_C(1A.GND, GND)
	NET_C(1A.VCC, C101.1, R241.1, R240.1)

	DIODE(CR22, "1N4454")

	NET_C(NOISE.1, R241.2, 1A.PLUS)
	NET_C(NOISE.2, C99.1, GND)

	// White Noise
	NET_C(V15, R240.2, R246.1)
	NET_C(C99.2, R242.1)
	NET_C(R242.2, R243.1, CR22.K, 1A.MINUS)
	NET_C(CR22.A, R244.1)
	NET_C(R243.2, R244.2, 1A.OUT, R245.1)
	NET_C(C101.2, GND)
	NET_C(R245.2, C102.1)
	NET_C(C102.2, R248.1, Q9.B)
	NET_C(Q9.E, GND)
	NET_C(Q9.C, R248.2, R247.1)
	NET_C(R247.2, R246.2, C103.1)
	NET_C(C103.2, GND)
	ALIAS(WHITE_NOISE, Q9.C)
#endif

	SUBMODEL(PLAYER_CRASH, PLAYER_1_CRASH)
	NET_C(PLAYER_1_CRASH.NOISE_IN, WHITE_NOISE)
	NET_C(PLAYER_1_CRASH.CRASH_IN, PL1_CRASH.Q)
	NET_C(PLAYER_1_CRASH.V15, V15)
	NET_C(PLAYER_1_CRASH.VCC, VCC)
	NET_C(PLAYER_1_CRASH.GND, GND)
NETLIST_END()