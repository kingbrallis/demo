Library IEEE;
Library work;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
use work.CPU_Package.all;
use work.all;

ENTITY CPU is 
	    PORT( adr 	  : OUT address_bus; 
			  data    : IN instruction_bus; 
			  stop    : IN std_logic; 	-- stops statemachine 
			  --RWM_data: INOUT data_bus;
			  data_in : IN data_bus; 
			  data_out: OUT data_bus;
			  rw_RWM  : OUT std_logic; 	-- read on high 
			  ROM_en  : OUT std_logic; 	-- active low 
			  RWM_en  : OUT std_logic; 	-- active low 
			  clk 	  : IN std_logic; 
			  reset   : IN std_logic); 	-- active high 
END ENTITY CPU;
 
ARCHITECTURE Structure of CPU is 

Component Multiplexer
    Port ( Sel 		 : IN std_logic_vector(1 downto 0); 
		   Data_in_2 : IN data_word; 
		   Data_in_1 : IN data_bus; -- Potential type problem... 
		   Data_in_0 : IN data_word; 
		   Data_out  : OUT data_word);
end Component Multiplexer;

Component Controller 
    Port ( adr 	    : OUT address_bus; 		-- unsigned 
		   data 	: IN  program_word;     -- unsigned 
		   rw_RWM   : OUT std_logic;        -- read on high 
		   RWM_en   : OUT std_logic; 	    -- active low 
		   ROM_en   : OUT std_logic; 		-- active low 
	       clk 	    : IN  std_logic; 
		   reset 	: IN  std_logic; 		-- active high
           stop     : IN std_logic; 		-- stops statemachine 		   
		   rw_reg   : OUT std_logic; 	    -- read on high 
		   sel_op_1 : OUT std_logic_vector (1 downto 0); 
		   sel_op_0 : OUT std_logic_vector (1 downto 0); 
		   sel_in   : OUT std_logic_vector (1 downto 0); 
		   sel_mux  : OUT std_logic_vector (1 downto 0); 
		   alu_op   : OUT std_logic_vector (2 downto 0); 
		   alu_en   : OUT std_logic; 		-- active high 
		   z_flag   : IN  std_logic; 	    -- active high 
		   n_flag   : IN  std_logic; 	    -- active high 
		   o_flag   : IN  std_logic;        -- active high 
		   out_en   : OUT std_logic; 	    -- active high 
		   data_imm : OUT data_word); 	    -- signed  
end Component Controller;

Component RegisterFile
    PORT( clk       : 	IN std_logic; 
		  data_in   :  	IN data_word; 
		  data_out_1: 	OUT data_word; 
		  data_out_0: 	OUT data_word; 
		  sel_in    : 	IN std_logic_vector (1 downto 0); 
		  sel_out_1 : 	IN std_logic_vector (1 downto 0); 
		  sel_out_0 : 	IN std_logic_vector (1 downto 0); 
		  rw_reg    :   IN std_logic); 
end Component RegisterFile;

Component ALU
    PORT( Op    : 	IN std_logic_vector(2 downto 0); 
		  A     : 	IN data_word; 
		  B     : 	IN data_word; 
		  En    : 	IN std_logic; 
		  clk   : 	IN std_logic; 
		  y 	: 	OUT data_word; 
		  n_flag:   OUT std_logic; 
		  z_flag:   OUT std_logic; 
		  o_flag:   OUT std_logic); 
end Component ALU;

Component DataBuffer
    PORT ( out_en 	: IN std_logic; 
		   data_in  : IN data_word; 
		   data_out : OUT data_bus);
end Component DataBuffer;

---------------------------------------------------
Signal main_clk : std_logic;

--MUX
Signal MUX_Sel 		 : std_logic_vector(1 downto 0); 
Signal MUX_Data_in_2 : data_word; 
Signal MUX_Data_in_1 : data_bus; -- Potential type problem... 
Signal MUX_Data_in_0 : data_word; 
Signal MUX_Data_out  : data_word;

--Controller
Signal CTRL_adr 	 : address_bus; 	-- unsigned 
Signal CTRL_data 	 : program_word;   	-- unsigned 
Signal CTRL_rw_RWM   : std_logic;      	-- read on high 
Signal CTRL_RWM_en   : std_logic; 	    -- active low 
Signal CTRL_ROM_en   : std_logic; 		-- active low 
Signal CTRL_reset 	 : std_logic; 		-- active high 
Signal CTRL_stop     : std_logic;       -- stops statemachine
Signal CTRL_rw_reg   : std_logic; 	    -- read on high 
Signal CTRL_sel_op_1 : std_logic_vector (1 downto 0); 
Signal CTRL_sel_op_0 : std_logic_vector (1 downto 0); 
Signal CTRL_sel_in   : std_logic_vector (1 downto 0); 
Signal CTRL_sel_mux  : std_logic_vector (1 downto 0); 
Signal CTRL_alu_op   : std_logic_vector (2 downto 0); 
Signal CTRL_alu_en   : std_logic; 		-- active high 
Signal CTRL_z_flag   : std_logic; 	    -- active high 
Signal CTRL_n_flag   : std_logic; 	    -- active high 
Signal CTRL_o_flag   : std_logic;      	-- active high 
Signal CTRL_out_en   : std_logic; 	    -- active high 
Signal CTRL_data_imm : data_word; 		-- signed  

--RegisterFile
Signal REG_data_in   : data_word; 
Signal REG_data_out_1: data_word; 
Signal REG_data_out_0: data_word; 
Signal REG_sel_in    : std_logic_vector (1 downto 0); 
Signal REG_sel_out_1 : std_logic_vector (1 downto 0); 
Signal REG_sel_out_0 : std_logic_vector (1 downto 0); 
Signal REG_rw_reg    : std_logic; 

--ALU
Signal ALU_Op     : std_logic_vector(2 downto 0); 
Signal ALU_A      : data_word; 
Signal ALU_B      : data_word; 
Signal ALU_En     : std_logic; 
Signal ALU_y 	  : data_word; 
Signal ALU_n_flag : std_logic; 
Signal ALU_z_flag : std_logic; 
Signal ALU_o_flag : std_logic; 

--DataBuffer
Signal Databffr_out_en 	 : std_logic; 
Signal Databffr_data_in  : data_word; 
Signal Databffr_data_out : data_bus;

---------------------------------------------------
Begin
	 MUX0: Multiplexer PORT MAP( Sel 	   => MUX_Sel,
								 Data_in_2 => MUX_Data_in_2,
								 Data_in_1 => MUX_Data_in_1,
								 Data_in_0 => MUX_Data_in_0,
								 Data_out  => MUX_Data_out);
	 
	 CNTLR0: Controller PORT MAP( adr	   => CTRL_adr,
								  data	   => CTRL_data,
								  rw_RWM   => CTRL_rw_RWM,
								  RWM_en   => CTRL_RWM_en,
								  ROM_en   => CTRL_ROM_en,
								  reset    => CTRL_reset,
								  stop     => CTRL_stop,
								  rw_reg   => CTRL_rw_reg,
								  sel_op_1 => CTRL_sel_op_1,
								  sel_op_0 => CTRL_sel_op_0,
								  sel_in   => CTRL_sel_in,
								  sel_mux  => CTRL_sel_mux,
								  alu_op   => CTRL_alu_op,
								  alu_en   => CTRL_alu_en,
								  z_flag   => CTRL_z_flag,
								  n_flag   => CTRL_n_flag,
								  o_flag   => CTRL_o_flag,
								  out_en   => CTRL_out_en,
								  clk      => main_clk,
								  data_imm => CTRL_data_imm);
								  
	REG0: RegisterFile PORT MAP( clk       => main_clk,
								 data_in   => REG_data_in,
								 data_out_1=> REG_data_out_1,
								 data_out_0=> REG_data_out_0,
								 sel_in    => REG_sel_in,
								 sel_out_1 => REG_sel_out_1,
								 sel_out_0 => REG_sel_out_0,
								 rw_reg    => REG_rw_reg);
								 
	ALU0: ALU 		   PORT MAP( Op		   => ALU_Op,
								 A		   => ALU_A,
								 B		   => ALU_B,
								 En		   => ALU_En,
								 clk       => main_clk,
								 y		   => ALU_y,
								 n_flag    => ALU_n_flag,
								 z_flag    => ALU_z_flag,
								 o_flag    => ALU_o_flag);

	Databffr: DataBuffer PORT MAP(out_en   => Databffr_out_en,
								  data_in  => Databffr_data_in,
								  data_out => Databffr_data_out);
								 
---------------------------------------------------
	main_clk 		  <= clk;
	CTRL_reset		  <= reset;
	CTRL_stop         <= stop;
	adr		   	      <= CTRL_adr;
	data_out          <= Databffr_data_out; --RWM_data
	
	MUX_Sel  		  <= CTRL_sel_mux;
	MUX_Data_in_0     <= ALU_y;
	MUX_Data_in_2     <= CTRL_data_imm;
	MUX_Data_in_1     <= data_in; --RWM_data
	
	REG_data_in 	  <= MUX_Data_out;
	REG_sel_in 		  <= CTRL_sel_in;
	REG_sel_out_1 	  <= CTRL_sel_op_1;
	REG_sel_out_0     <= CTRL_sel_op_0;
	REG_rw_reg 		  <= CTRL_rw_reg;
	
	CTRL_z_flag       <= ALU_z_flag;
	CTRL_n_flag       <= ALU_n_flag;
	CTRL_o_flag       <= ALU_o_flag;
	CTRL_data		  <= data;
	
	ALU_En 			  <= CTRL_alu_en;
	ALU_A 			  <= REG_data_out_1;
	ALU_B 			  <= REG_data_out_0;
	ALU_Op 			  <= CTRL_alu_op;
	
	Databffr_out_en   <= CTRL_out_en;
	Databffr_data_in  <= REG_data_out_1;
	
	rw_RWM            <= CTRL_rw_RWM;
	RWM_en            <= CTRL_RWM_en;
	ROM_en            <= CTRL_ROM_en;
	
end Structure;	