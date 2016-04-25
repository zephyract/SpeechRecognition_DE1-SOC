// Copyright (C) 1991-2015 Altera Corporation. All rights reserved.
// Your use of Altera Corporation's design tools, logic functions 
// and other software and tools, and its AMPP partner logic 
// functions, and any output files from any of the foregoing 
// (including device programming or simulation files), and any 
// associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License 
// Subscription Agreement, the Altera Quartus II License Agreement,
// the Altera MegaCore Function License Agreement, or other 
// applicable license agreement, including, without limitation, 
// that your use is for the sole purpose of programming logic 
// devices manufactured by Altera and sold by Altera or its 
// authorized distributors.  Please refer to the applicable 
// agreement for further details.


// Generated by Quartus II 64-Bit Version 15.0 (Build Build 145 04/22/2015)
// Created on Mon Apr 25 12:34:23 2016

LtoH LtoH_inst
(
	.LCLK(LCLK_sig) ,	// input  LCLK_sig
	.HCLK(HCLK_sig) ,	// input  HCLK_sig
	.Din(Din_sig) ,	// input [datos-1:0] Din_sig
	.Dout(Dout_sig) 	// output [datos-1:0] Dout_sig
);

defparam LtoH_inst.datos = 16;
