library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity regBank is
    Port (
    clk : in std_logic;
    Addr1, Addr2    : in  signed (4 downto 0);
    WriteAddr : in signed(4 downto 0);
    Write : in  STD_LOGIC;
    Din   : in  signed (31 downto 0);
    Dout1, Dout2 : out signed (31 downto 0)
    );
end regBank;

architecture behavior of regBank is
    type bankArr is array(0 to 31) of signed(31 downto 0);
    signal bank : bankArr;   
    begin
      Dout1 <= bank(to_integer(unsigned(Addr1)));
      Dout2 <= bank(to_integer(unsigned(Addr2)));
      process begin
        if rising_edge(clk) then
          if (Write='1') then
            bank(to_integer(unsigned(WriteAddr))) <= Din;
          end if;
        end if;
      end process;
    end behavior;
