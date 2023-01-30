LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY pre_wb_reg IS
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
END ENTITY pre_wb_reg;

ARCHITECTURE behavior OF pre_wb_reg IS
BEGIN
    pre_wb_P : PROCESS (clk, reset) IS
    BEGIN
        IF reset = '1' THEN
            res_out <= (OTHERS => '0');
            dest_addr_out <= (OTHERS => '0');
            reg_write_out <= '0';
            halt_out <= '0';
        END IF;

        IF rising_edge(clk) THEN
            IF memory_read_in = '1' THEN
                res_out <= mem_val_in;
            ELSE
                res_out <= res_in;
            END IF;
            dest_addr_out <= dest_addr_in;
            reg_write_out <= reg_write_in;
            halt_out <= halt_in;
        END IF;
    END PROCESS pre_wb_P;
END ARCHITECTURE behavior;