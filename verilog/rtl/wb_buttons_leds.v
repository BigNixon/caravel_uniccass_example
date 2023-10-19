`default_nettype none
`timescale 1ns/1ns

module wb_buttons_leds #(
    parameter   [31:0]  BASE_ADDRESS    = 32'h3000_0000,        // base address
    parameter   [31:0]  SUMA_ADDRESS     = BASE_ADDRESS,
    parameter   [31:0]  SUMB_ADDRESS     = BASE_ADDRESS + 12,
    parameter   [31:0]  BUTTON_ADDRESS  = BASE_ADDRESS + 4,
    parameter   [31:0]  OPCODE_ADDRESS = BASE_ADDRESS + 16,
    parameter   [31:0]  SALIDA_ADDRESS  = BASE_ADDRESS + 8
    ) (
`ifdef USE_POWER_PINS
    inout vccd1,	// User area 1 1.8V supply
    inout vssd1,	// User area 1 digital ground
`endif
    input wire          clk,
    input wire          reset,

    // wb interface
    input wire          i_wb_cyc,       // wishbone transaction
    input wire          i_wb_stb,       // strobe - data valid and accepted as long as !o_wb_stall
    input wire          i_wb_we,        // write enable
    input wire  [31:0]  i_wb_addr,      // address
    input wire  [31:0]  i_wb_data,      // incoming data
    output reg          o_wb_ack,       // request is completed 
    output wire         o_wb_stall,     // cannot accept req
    output reg  [31:0]  o_wb_data,      // output data
    // buttons
    input wire  [2:0]   buttons,
    output wire [7:0] led_enb,
    output reg [3:0] leds
 
    );
    reg [31:0] salida;
    reg [31:0] sum_a;
    reg [31:0] sum_b;
    assign o_wb_stall = 0;
    reg [2:0] op_code;
    //initial leds = 8'b0;

    assign led_enb = 8'b0;
    wire [7:0] data_a_out;

    /////////////////

    reg [3:0] LowA;     
    reg [3:0] HighA;    
    reg [3:0] reg_data_A=0;
    
    always @(posedge clk) begin
        case (op_code)
            3'b000:
                salida = ~sum_a;
            3'b001:
                salida = sum_a & sum_b;
            3'b010:
                salida = sum_a;
            3'b011:
                salida = sum_a | sum_b;
            3'b100:
                salida = sum_a-1;
            3'b101:
                salida = sum_a + sum_b;
            3'b110:
                salida = sum_a + ~sum_b + 1;
            3'b111:
                salida = sum_a + 1;
            default: 
                salida = 'b0;
        endcase
    end

    

    always @(posedge clk) begin
        if(reset) begin
            sum_a <= 32'b0;
            sum_b <= 32'b0;
        end else if(i_wb_stb && i_wb_cyc && i_wb_we && !o_wb_stall && i_wb_addr == SUMA_ADDRESS) begin
            sum_a <= i_wb_data[31:0];
        end else if(i_wb_stb && i_wb_cyc && i_wb_we && !o_wb_stall && i_wb_addr == SUMB_ADDRESS) begin
            sum_b <= i_wb_data[31:0];
        end else if(i_wb_stb && i_wb_cyc && i_wb_we && !o_wb_stall && i_wb_addr == OPCODE_ADDRESS) begin
            op_code <= i_wb_data[31:0];
        end
        leds <= sum_a[3:0];
    end

    // reads
    always @(posedge clk) begin
        //salida<=sum_a+ sum_b;
        //reg_data_A <= sum_a;
        if(reset)
            o_wb_data <= 0;
        else if(i_wb_stb && i_wb_cyc && !i_wb_we && !o_wb_stall)
            case(i_wb_addr)
                SALIDA_ADDRESS: 
                    o_wb_data <= {salida};
                BUTTON_ADDRESS: 
                    o_wb_data <= {29'b0, buttons};
                default:
                    o_wb_data <= 32'b0;
            endcase

    end

    // acks
    always @(posedge clk) begin
        if(reset)
            o_wb_ack <= 0;
        else
            // return ack immediately
            o_wb_ack <= (i_wb_stb && !o_wb_stall && (i_wb_addr == OPCODE_ADDRESS ||  i_wb_addr == SUMA_ADDRESS || 
                                                    i_wb_addr == SUMB_ADDRESS || i_wb_addr == SALIDA_ADDRESS || 
                                                    i_wb_addr == BUTTON_ADDRESS));
    end

endmodule