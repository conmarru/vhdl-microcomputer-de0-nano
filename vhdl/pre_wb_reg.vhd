library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity pre_wb_reg is
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
end entity pre_wb_reg;

architecture behavior of pre_wb_reg is
    begin 
    pre_wb_P: process (clk, reset) is
    begin
        if reset = '1' then
            res_out <= (others =>'0');
            dest_addr_out <= (others => '0');
            reg_write_out <= '0';
        end if;

        if rising_edge(clk) then
            if memory_read_in = '1' then
                res_out <= mem_val_in;
            else
                res_out <= res_in;
            end if;
            dest_addr_out <= dest_addr_in;
            reg_write_out <= reg_write_in;
        end if;
    end process pre_wb_P;
end architecture behavior;