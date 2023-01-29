library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity cpu is
end entity;

architecture cpu_go_fast of cpu is

    component branch is
    port (
        condition : in signed(31 downto 0);
        cond_func : in signed(2 downto 0);
        do_branch : out std_logic
    );
    end component branch;

    component alu is
        port (
        --clk : in std_logic;    
        A, B: in signed(31 downto 0);
        opcode : in signed(4 downto 0);
        func : in signed(4 downto 0);
        C: out signed(31 downto 0) 
        );
    end component alu ;

    component pre_exec_reg is
        port(
            -- control
            reset : in std_logic;
            clk : in std_logic;
            -- in
            next_pc_in, val_a_in, val_b_in, val_d_in : in signed(31 downto 0);
            opcode_in, dest_addr_in, func_in : in signed(4 downto 0);
            imm_in : in signed(31 downto 0);
            use_imm_in : in std_logic;
            use_jmp_in : in std_logic;
            reg_write_in : in std_logic;
            memory_read_in, memory_write_in : in std_logic;
            -- out
            next_pc_out, val_a_out, val_b_out, val_d_out : out signed(31 downto 0);
            opcode_out, dest_addr_out, func_out : out signed(4 downto 0);
            imm_out : out signed(31 downto 0);
            use_imm_out : out std_logic;
            use_jmp_out : out std_logic;
            reg_write_out : out std_logic;
            memory_read_out, memory_write_out : out std_logic
        );
    end component pre_exec_reg;

    component pre_fetch_reg is
        port(
            clk : in std_logic;
            next_pc_in, instruction : in signed(31 downto 0);
            reset : in std_logic;
            next_pc_out : out signed(31 downto 0);
            opcode_out, addr_a_out, addr_b_out, addr_dest_out, func_out : out signed(4 downto 0);
            imm_out : out signed(31 downto 0);
            use_imm_out : out std_logic;
            use_jmp_out : out std_logic;
            reg_write_out : out std_logic;
            memory_read_out, memory_write_out : out std_logic
        );
    end component pre_fetch_reg;

    component pre_mem_reg is
        port(
            -- control
            reset : in std_logic;
            clk : in std_logic;
            -- in
            next_pc_in, res_in, val_d_in : in signed(31 downto 0);
            dest_addr_in : in signed(4 downto 0);
            use_jmp_in : in std_logic;
            use_branch_in : in std_logic;
            reg_write_in : in std_logic;
            memory_read_in, memory_write_in : in std_logic;
            -- out
            next_pc_out, res_out, val_d_out : out signed(31 downto 0);
            dest_addr_out : out signed(4 downto 0);
            use_jmp_out : out std_logic;
            use_branch_out : out std_logic;
            reg_write_out : out std_logic;
            memory_read_out, memory_write_out : out std_logic
        );
    end component pre_mem_reg;

    component pre_wb_reg is
        port(
        clk : in std_logic;
        reset : in std_logic;
        res_in, mem_val_in : in signed(31 downto 0);
        dest_addr_in : in signed(4 downto 0);
        reg_write_in : in std_logic;
        memory_read_in : in std_logic;
        res_out : out signed(31 downto 0);
        dest_addr_out : out signed(4 downto 0);
        reg_write_out : out std_logic
        );
    end component pre_wb_reg;
    
    component program_counter is
        Port (
            -- control
            reset : in std_logic;
            clk : in std_logic;
            -- in
            used_jmp : in std_logic;
            used_branch : in std_logic;
            halt : in std_logic;
            next_pc_in : in signed(31 downto 0);
            -- out
            pc : out signed(31 downto 0);
            next_pc_out : out signed(31 downto 0)
        );
    end component program_counter;

    component RAM is
        Generic (
               Addrbreite  : natural := 32;  -- Speicherlänge = 2^Addrbreite
               Wortbreite  : natural := 32
               );
        Port ( clk   : in  STD_LOGIC;
               Addr     : in  signed (Addrbreite-1 downto 0);
               Write : in  STD_LOGIC;
               Din   : in  signed (Wortbreite-1 downto 0);
               Dout  : out signed (Wortbreite-1 downto 0)
             );
    end component RAM;

    component reg_bank is
        Port (
        clk : in std_logic;
        addr_a, addr_b, addr_dest    : in  signed (4 downto 0);
        addr_write : in signed(4 downto 0);
        write : in  STD_LOGIC;
        val_write   : in  signed (31 downto 0);
        val_a, val_b, val_res : out signed (31 downto 0)
        );
    end component reg_bank;
    
    signal clk : std_logic;
    signal reset : std_logic;

    signal f_instruction: signed(31 downto 0);
    signal f_next_pc : signed(31 downto 0);

    signal d_next_pc : signed(31 downto 0);
    signal d_opcode : signed(4 downto 0);
    signal d_addr_a : signed(4 downto 0);
    signal d_addr_b : signed(4 downto 0);
    signal d_addr_dest : signed(4 downto 0);
    signal d_func : signed(4 downto 0);
    signal d_imm : signed(31 downto 0);
    signal d_use_imm : std_logic;
    signal d_use_jmp : std_logic;
    signal d_reg_write : std_logic;
    signal d_memory_read : std_logic;
    signal d_memory_write : std_logic;

    signal e_next_pc : signed(31 downto 0);
    signal e_val_a : signed(31 downto 0);
    signal e_val_b : signed(31 downto 0);
    signal e_val_d : signed(31 downto 0);
    signal e_opcode : signed(4 downto 0);
    signal e_dest_addr : signed(4 downto 0);
    signal e_func : signed(4 downto 0);
    signal e_imm : signed(31 downto 0);
    signal e_use_imm : std_logic;
    signal e_use_jmp : std_logic;
    signal e_reg_write : std_logic;
    signal e_memory_read : std_logic;
    signal e_memory_write : std_logic;
    
    signal m_next_pc : signed(31 downto 0);
    signal m_res : signed(31 downto 0);
    signal m_val_d : signed(31 downto 0);
    signal m_dest_addr : signed(4 downto 0);
    signal m_used_jmp : std_logic;
    signal m_used_branch : std_logic;
    signal m_reg_write : std_logic;
    signal m_memory_read : std_logic;
    signal m_memory_write : std_logic;

    signal w_res : signed(31 downto 0);
    signal w_dest_addr : signed(4 downto 0);
    signal w_reg_write : std_logic;

    signal alu_res : signed(31 downto 0);
    
    signal reg_val_a : signed(31 downto 0);
    signal reg_val_b : signed(31 downto 0);
    signal reg_val_res : signed(31 downto 0);

    signal instrmem_instr : signed(31 downto 0);
    
    signal mem_val : signed(31 downto 0);

    signal branch_use_branch : std_logic;

    signal pc_pc : signed(31 downto 0);
    signal pc_next_pc : signed(31 downto 0);

    signal nein :std_logic := '0';
    signal nein_aber_lang :signed(31 downto 0) := (others => '0');

begin
    instr_mem: entity work.ram
    port map(clk, reset, pc_pc, nein, nein_aber_lang, instrmem_instr);
    mem: entity work.ram
    port map(clk, reset, m_res, m_memory_write, m_val_d, mem_val);

    pc: entity work.program_counter
    -- todo: nein, m_next_pc muss schon fallunterscheidungen machen
    port map(reset, clk, m_used_jmp, m_used_branch, nein, m_next_pc, pc_pc, pc_next_pc);

    alu_dings: entity work.alu
    port map(e_val_a, e_val_b, e_opcode, e_func, alu_res);

    branch_dings: entity work.branch
    port map(e_val_d, e_func, branch_use_branch);

    pre_fetch_reg_dings: entity work.pre_fetch_reg
    port map(clk, pc_next_pc, instrmem_instr, reset, 
        d_next_pc, d_opcode, d_addr_a, d_addr_b, d_addr_dest, 
        d_func, d_imm, d_use_imm, d_use_jmp, d_reg_write, d_memory_read, 
        d_memory_write);

    pre_exec_reg_dings: entity work.pre_exec_reg
    port map(reset, clk, d_next_pc, reg_val_a, reg_val_b, reg_val_res,
        d_opcode, d_addr_dest, d_func, d_imm, d_use_imm, d_use_jmp,
        d_reg_write, d_memory_read, d_memory_write,
        e_next_pc, e_val_a, e_val_b, e_val_d, e_opcode, e_dest_addr,
        e_func, e_imm, e_use_imm, e_use_jmp, e_reg_write, e_memory_read,
        e_memory_write);
    
    reg_bank_dings: entity work.reg_bank
    port map(clk, d_addr_a, d_addr_b, d_addr_dest, w_dest_addr, w_reg_write, w_res,
        reg_val_a, reg_val_b, reg_val_res);

    pre_mem_reg_dings: entity work.pre_mem_reg
    port map(reset, clk, e_next_pc, alu_res, e_val_d, e_dest_addr,
        e_use_jmp, branch_use_branch, e_reg_write, e_memory_read, e_memory_write,
        m_next_pc, m_res, m_val_d, m_dest_addr, m_used_jmp, m_used_branch,
        m_reg_write, m_memory_read, m_memory_write);

    pre_wb_reg_dings: entity work.pre_wb_reg
    port map(clk, reset, m_res, mem_val, m_dest_addr, m_reg_write, m_memory_read, 
        w_res, w_dest_addr, w_reg_write);
    

    stiP: process is
    begin
        wait for 20 ms;
        clk <= '0';
        reset <= '1';
        wait for 1 ms;
        reset <= '0';
        wait for 20 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;
        clk <= '1';
        wait for 1 ms;
        clk <= '0';
        wait for 1 ms;

        wait;
    end process stiP;
end architecture cpu_go_fast;
