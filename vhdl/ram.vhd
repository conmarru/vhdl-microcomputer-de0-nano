library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
-- yoinked from lothar-miller.de
entity RAM is
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
end RAM;

architecture BlockRAM of RAM is
    type speicher is array(0 to (2**Addrbreite)-1) of signed(Wortbreite-1 downto 0);
    signal memory : speicher;   
    begin
      process begin
        wait until rising_edge(CLK);
        if (Write='1') then
          memory(to_integer(unsigned(A))) <= Din;
        end if;
        Dout <= memory(to_integer(unsigned(A)));
      end process;
    end BlockRAM;