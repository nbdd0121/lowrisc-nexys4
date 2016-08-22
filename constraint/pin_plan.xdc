# on board single-end clock, 100MHz
set_property PACKAGE_PIN E3 [get_ports clk_p]
set_property IOSTANDARD LVCMOS33 [get_ports clk_p]

# Reset active high SW4.1 User button South
set_property IOSTANDARD LVCMOS33 [get_ports {rst_top}]
set_property LOC C12 [get_ports {rst_top}]

# UART Pins
set_property PACKAGE_PIN C4 [get_ports rxd]
set_property IOSTANDARD LVCMOS33 [get_ports rxd]
set_property PACKAGE_PIN D4 [get_ports txd]
set_property IOSTANDARD LVCMOS33 [get_ports txd]
set_property PACKAGE_PIN E5 [get_ports cts]
set_property IOSTANDARD LVCMOS33 [get_ports cts]
set_property PACKAGE_PIN D3 [get_ports rts]
set_property IOSTANDARD LVCMOS33 [get_ports rts]

# SD/SPI Pins
set_property PACKAGE_PIN D2 [get_ports spi_cs]
set_property IOSTANDARD LVCMOS33 [get_ports spi_cs]
set_property PACKAGE_PIN B1 [get_ports spi_sclk]
set_property IOSTANDARD LVCMOS33 [get_ports spi_sclk]
set_property PACKAGE_PIN C1 [get_ports spi_mosi]
set_property IOSTANDARD LVCMOS33 [get_ports spi_mosi]
set_property PACKAGE_PIN C2 [get_ports spi_miso]
set_property IOSTANDARD LVCMOS33 [get_ports spi_miso]
set_property PACKAGE_PIN E2 [get_ports sd_reset]
set_property IOSTANDARD LVCMOS33 [get_ports sd_reset]

# VGA
set_property PACKAGE_PIN A4 [get_ports vga_red[3]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_red[3]]
set_property PACKAGE_PIN C5 [get_ports vga_red[2]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_red[2]]
set_property PACKAGE_PIN B4 [get_ports vga_red[1]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_red[1]]
set_property PACKAGE_PIN A3 [get_ports vga_red[0]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_red[0]]
set_property PACKAGE_PIN A6 [get_ports vga_green[3]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_green[3]]
set_property PACKAGE_PIN B6 [get_ports vga_green[2]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_green[2]]
set_property PACKAGE_PIN A5 [get_ports vga_green[1]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_green[1]]
set_property PACKAGE_PIN C6 [get_ports vga_green[0]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_green[0]]
set_property PACKAGE_PIN D8 [get_ports vga_blue[3]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_blue[3]]
set_property PACKAGE_PIN D7 [get_ports vga_blue[2]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_blue[2]]
set_property PACKAGE_PIN C7 [get_ports vga_blue[1]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_blue[1]]
set_property PACKAGE_PIN B7 [get_ports vga_blue[0]]
set_property IOSTANDARD LVCMOS33 [get_ports vga_blue[0]]
set_property PACKAGE_PIN B11 [get_ports vga_hsync]
set_property IOSTANDARD LVCMOS33 [get_ports vga_hsync]
set_property PACKAGE_PIN B12 [get_ports vga_vsync]
set_property IOSTANDARD LVCMOS33 [get_ports vga_vsync]

# Flash/SPI Pins
set_property PACKAGE_PIN L13 [get_ports flash_ss]
set_property IOSTANDARD LVCMOS33 [get_ports flash_ss]
set_property PACKAGE_PIN K17 [get_ports flash_io[0]]
set_property IOSTANDARD LVCMOS33 [get_ports flash_io[0]]
set_property PACKAGE_PIN K18 [get_ports flash_io[1]]
set_property IOSTANDARD LVCMOS33 [get_ports flash_io[1]]
set_property PACKAGE_PIN L14 [get_ports flash_io[2]]
set_property IOSTANDARD LVCMOS33 [get_ports flash_io[2]]
set_property PACKAGE_PIN M14 [get_ports flash_io[3]]
set_property IOSTANDARD LVCMOS33 [get_ports flash_io[3]]
