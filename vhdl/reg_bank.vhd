library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity reg_bank is
    Port (
    clk : in std_logic;
    addr_a, addr_b, addr_dest    : in  signed (4 downto 0);
    addr_write : in signed(4 downto 0);
    write : in  STD_LOGIC;
    val_write   : in  signed (31 downto 0);
    val_a, val_b, val_res : out signed (31 downto 0)
    );
end reg_bank;

architecture behavior of reg_bank is
    type bankArr is array(0 to 31) of signed(31 downto 0);
    signal bank : bankArr;   
    begin
      val_a <= bank(to_integer(unsigned(addr_a)));
      val_b <= bank(to_integer(unsigned(addr_b)));
      val_res <= bank(to_integer(unsigned(addr_dest)));
      process (clk) is 
      begin
        if rising_edge(clk) then
          if (write = '1') then
            bank(to_integer(unsigned(addr_write))) <= val_write;
          end if;
        end if;
      end process;
    end behavior;
