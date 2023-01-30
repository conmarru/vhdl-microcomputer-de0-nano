LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY cpu IS
END ENTITY;

ARCHITECTURE cpu_go_fast OF cpu IS

    COMPONENT branch IS
        PORT (
            condition : IN signed(31 DOWNTO 0);
            cond_func : IN signed(2 DOWNTO 0);
            do_branch : OUT STD_LOGIC
        );
    END COMPONENT branch;

    COMPONENT alu IS
        PORT (
            --clk : in std_logic;    
            A, B : IN signed(31 DOWNTO 0);
            opcode : IN signed(4 DOWNTO 0);
            func : IN signed(4 DOWNTO 0);
            C : OUT signed(31 DOWNTO 0)
        );
    END COMPONENT alu;

    COMPONENT pre_exec_reg IS
        PORT (
            -- control
            reset : IN STD_LOGIC;
            clk : IN STD_LOGIC;
            -- in
            pc_in, next_pc_in, val_a_in, val_b_in, val_d_in : IN signed(31 DOWNTO 0);
            opcode_in, dest_addr_in, func_in : IN signed(4 DOWNTO 0);
            imm_in : IN signed(31 DOWNTO 0);
            use_imm_in : IN STD_LOGIC;
            use_jmp_in : IN STD_LOGIC;
            reg_write_in : IN STD_LOGIC;
            memory_read_in, memory_write_in : IN STD_LOGIC;
            halt_in : IN STD_LOGIC;
            -- out
            next_pc_out, val_a_out, val_b_out, val_d_out : OUT signed(31 DOWNTO 0);
            opcode_out, dest_addr_out, func_out : OUT signed(4 DOWNTO 0);
            imm_out : OUT signed(31 DOWNTO 0);
            use_imm_out : OUT STD_LOGIC;
            use_jmp_out : OUT STD_LOGIC;
            reg_write_out : OUT STD_LOGIC;
            memory_read_out, memory_write_out : OUT STD_LOGIC;
            halt_out : OUT STD_LOGIC
        );
    END COMPONENT pre_exec_reg;

    COMPONENT pre_fetch_reg IS
        PORT (
            clk : IN STD_LOGIC;
            pc_in, next_pc_in, instruction : IN signed(31 DOWNTO 0);
            reset : IN STD_LOGIC;
            pc_out, next_pc_out : OUT signed(31 DOWNTO 0);
            opcode_out, addr_a_out, addr_b_out, addr_dest_out, func_out : OUT signed(4 DOWNTO 0);
            imm_out : OUT signed(31 DOWNTO 0);
            use_imm_out : OUT STD_LOGIC;
            use_jmp_out : OUT STD_LOGIC;
            reg_write_out : OUT STD_LOGIC;
            memory_read_out, memory_write_out : OUT STD_LOGIC;
            halt_out : OUT STD_LOGIC
        );
    END COMPONENT pre_fetch_reg;

    COMPONENT pre_mem_reg IS
        PORT (
            -- control
            reset : IN STD_LOGIC;
            clk : IN STD_LOGIC;
            -- in
            next_pc_in, res_in, val_d_in : IN signed(31 DOWNTO 0);
            dest_addr_in : IN signed(4 DOWNTO 0);
            use_jmp_in : IN STD_LOGIC;
            use_branch_in : IN STD_LOGIC;
            reg_write_in : IN STD_LOGIC;
            memory_read_in, memory_write_in : IN STD_LOGIC;
            halt_in : IN STD_LOGIC;
            -- out
            next_pc_out, res_out, val_d_out : OUT signed(31 DOWNTO 0);
            dest_addr_out : OUT signed(4 DOWNTO 0);
            use_jmp_out : OUT STD_LOGIC;
            use_branch_out : OUT STD_LOGIC;
            reg_write_out : OUT STD_LOGIC;
            memory_read_out, memory_write_out : OUT STD_LOGIC;
            halt_out : OUT STD_LOGIC
        );
    END COMPONENT pre_mem_reg;

    COMPONENT pre_wb_reg IS
        PORT (
            clk : IN STD_LOGIC;
            reset : IN STD_LOGIC;
            res_in, mem_val_in : IN signed(31 DOWNTO 0);
            dest_addr_in : IN signed(4 DOWNTO 0);
            reg_write_in : IN STD_LOGIC;
            memory_read_in : IN STD_LOGIC;
            halt_in : IN STD_LOGIC;
            res_out : OUT signed(31 DOWNTO 0);
            dest_addr_out : OUT signed(4 DOWNTO 0);
            reg_write_out : OUT STD_LOGIC;
            halt_out : OUT STD_LOGIC
        );
    END COMPONENT pre_wb_reg;

    COMPONENT program_counter IS
        PORT (
            -- control
            reset : IN STD_LOGIC;
            clk : IN STD_LOGIC;
            -- in
            used_jmp : IN STD_LOGIC;
            used_branch : IN STD_LOGIC;
            next_pc_in : IN signed(31 DOWNTO 0);
            -- out
            pc : OUT signed(31 DOWNTO 0);
            next_pc_out : OUT signed(31 DOWNTO 0)
        );
    END COMPONENT program_counter;

    COMPONENT RAM IS
        GENERIC (
            Addrbreite : NATURAL := 32; -- Speicherlänge = 2^Addrbreite
            Wortbreite : NATURAL := 32
        );
        PORT (
            clk : IN STD_LOGIC;
            Addr : IN signed (Addrbreite - 1 DOWNTO 0);
            Write : IN STD_LOGIC;
            Din : IN signed (Wortbreite - 1 DOWNTO 0);
            Dout : OUT signed (Wortbreite - 1 DOWNTO 0)
        );
    END COMPONENT RAM;

    COMPONENT reg_bank IS
        PORT (
            clk : IN STD_LOGIC;
            addr_a, addr_b, addr_dest : IN signed (4 DOWNTO 0);
            addr_write : IN signed(4 DOWNTO 0);
            write : IN STD_LOGIC;
            val_write : IN signed (31 DOWNTO 0);
            val_a, val_b, val_res : OUT signed (31 DOWNTO 0)
        );
    END COMPONENT reg_bank;

    SIGNAL clk : STD_LOGIC;
    SIGNAL reset : STD_LOGIC;
    SIGNAL halt : STD_LOGIC;

    -- pipeline output for decode
    SIGNAL d_pc : signed(31 DOWNTO 0);
    SIGNAL d_next_pc : signed(31 DOWNTO 0);
    SIGNAL d_opcode : signed(4 DOWNTO 0);
    SIGNAL d_addr_a : signed(4 DOWNTO 0);
    SIGNAL d_addr_b : signed(4 DOWNTO 0);
    SIGNAL d_addr_dest : signed(4 DOWNTO 0);
    SIGNAL d_func : signed(4 DOWNTO 0);
    SIGNAL d_imm : signed(31 DOWNTO 0);
    SIGNAL d_use_imm : STD_LOGIC;
    SIGNAL d_use_jmp : STD_LOGIC;
    SIGNAL d_reg_write : STD_LOGIC;
    SIGNAL d_memory_read : STD_LOGIC;
    SIGNAL d_memory_write : STD_LOGIC;
    SIGNAL d_halt : STD_LOGIC;

    -- pipeline output for execute
    SIGNAL e_pc : signed(31 DOWNTO 0);
    SIGNAL e_next_pc : signed(31 DOWNTO 0);
    SIGNAL e_val_a : signed(31 DOWNTO 0);
    SIGNAL e_val_b : signed(31 DOWNTO 0);
    SIGNAL e_val_d : signed(31 DOWNTO 0);
    SIGNAL e_opcode : signed(4 DOWNTO 0);
    SIGNAL e_dest_addr : signed(4 DOWNTO 0);
    SIGNAL e_func : signed(4 DOWNTO 0);
    SIGNAL e_imm : signed(31 DOWNTO 0);
    SIGNAL e_use_imm : STD_LOGIC;
    SIGNAL e_use_jmp : STD_LOGIC;
    SIGNAL e_reg_write : STD_LOGIC;
    SIGNAL e_memory_read : STD_LOGIC;
    SIGNAL e_memory_write : STD_LOGIC;
    SIGNAL e_halt : STD_LOGIC;

    -- pipeline output for memory
    SIGNAL m_next_pc : signed(31 DOWNTO 0);
    SIGNAL m_res : signed(31 DOWNTO 0);
    SIGNAL m_val_d : signed(31 DOWNTO 0);
    SIGNAL m_dest_addr : signed(4 DOWNTO 0);
    SIGNAL m_used_jmp : STD_LOGIC;
    SIGNAL m_used_branch : STD_LOGIC;
    SIGNAL m_reg_write : STD_LOGIC;
    SIGNAL m_memory_read : STD_LOGIC;
    SIGNAL m_memory_write : STD_LOGIC;
    SIGNAL m_halt : STD_LOGIC;

    -- pipeline output for writeback
    SIGNAL w_res : signed(31 DOWNTO 0);
    SIGNAL w_dest_addr : signed(4 DOWNTO 0);
    SIGNAL w_reg_write : STD_LOGIC;

    -- other component results
    SIGNAL alu_res : signed(31 DOWNTO 0);

    SIGNAL reg_val_a : signed(31 DOWNTO 0);
    SIGNAL reg_val_b : signed(31 DOWNTO 0);
    SIGNAL reg_val_res : signed(31 DOWNTO 0);

    SIGNAL instrmem_instr : signed(31 DOWNTO 0);

    SIGNAL mem_val : signed(31 DOWNTO 0);

    SIGNAL branch_use_branch : STD_LOGIC;

    SIGNAL pc_pc : signed(31 DOWNTO 0);
    SIGNAL pc_next_pc : signed(31 DOWNTO 0);

    SIGNAL zero : STD_LOGIC := '0';
    SIGNAL more_zeroes : signed(31 DOWNTO 0) := (OTHERS => '0');

BEGIN
    instr_mem : ENTITY work.ram
        PORT MAP(clk, reset, pc_pc, zero, more_zeroes, instrmem_instr);
    mem : ENTITY work.ram
        PORT MAP(clk, reset, m_res, m_memory_write, m_val_d, mem_val);

    pc : ENTITY work.program_counter
        PORT MAP(reset, clk, m_used_jmp, m_used_branch, m_next_pc, pc_pc, pc_next_pc);

    alu_dings : ENTITY work.alu
        PORT MAP(e_val_a, e_val_b, e_opcode, e_func, alu_res);

    branch_dings : ENTITY work.branch
        PORT MAP(e_val_d, e_opcode, e_func, branch_use_branch);

    pre_fetch_reg_dings : ENTITY work.pre_fetch_reg
        PORT MAP(
            clk, pc_pc, pc_next_pc, instrmem_instr, reset,
            d_pc, d_next_pc, d_opcode, d_addr_a, d_addr_b, d_addr_dest,
            d_func, d_imm, d_use_imm, d_use_jmp, d_reg_write, d_memory_read,
            d_memory_write, d_halt);
    d_memory_write, d_halt);

    pre_exec_reg_dings : ENTITY work.pre_exec_reg
        PORT MAP(
            reset, clk, d_pc, d_next_pc, reg_val_a, reg_val_b, reg_val_res,
            d_opcode, d_addr_dest, d_func, d_imm, d_use_imm, d_use_jmp,
            d_reg_write, d_memory_read, d_memory_write, d_halt,
            e_next_pc, e_val_a, e_val_b, e_val_d, e_opcode, e_dest_addr,
            e_func, e_imm, e_use_imm, e_use_jmp, e_reg_write, e_memory_read,
            e_memory_write, e_halt);
    e_memory_write, e_halt);

    reg_bank_dings : ENTITY work.reg_bank
        PORT MAP(
            clk, d_addr_a, d_addr_b, d_addr_dest, w_dest_addr, w_reg_write, w_res,
            reg_val_a, reg_val_b, reg_val_res);
    reg_val_a, reg_val_b, reg_val_res);

    pre_mem_reg_dings : ENTITY work.pre_mem_reg
        PORT MAP(
            reset, clk, e_next_pc, alu_res, e_val_d, e_dest_addr,
            e_use_jmp, branch_use_branch, e_reg_write, e_memory_read, e_memory_write, e_halt,
            m_next_pc, m_res, m_val_d, m_dest_addr, m_used_jmp, m_used_branch,
            m_reg_write, m_memory_read, m_memory_write, m_halt);
    m_reg_write, m_memory_read, m_memory_write, m_halt);

    pre_wb_reg_dings : ENTITY work.pre_wb_reg
        PORT MAP(
        clk, reset, m_res, mem_val, m_dest_addr, m_reg_write, m_memory_read, m_halt,
            w_res, w_dest_addr, w_reg_write, halt);
    w_res, w_dest_addr, w_reg_write, halt);
    stiP : PROCESS IS
        VARIABLE x : INTEGER := 0;
        BEGIN
        -- perform init
        WAIT FOR 1 ms;
        clk <= '0';
        reset <= '1';
        WAIT FOR 1 ms;
        reset <= '0';
        WAIT FOR 1 ms;

        WHILE halt = '0' LOOP
        clk <= '1';
            WAIT FOR 1 ms;
        clk <= '0';
            WAIT FOR 1 ms;
            x := x + 1;
            END LOOP;
        WAIT;
        END PROCESS stiP;