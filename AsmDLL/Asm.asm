;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Image filtering procedure with high-pass filter using 3x3 mask                                                                 
;                                                                                                                               
;2023/2024                                                                                                                      
;Author: Karol Skiba  
;Silesian University of Technology
;                                                                                                                               
;Description:                                                                                                                  
;	This procedure calculates the new value of each color channel of a 24-bit pixel 
;                               from a given array and stores it in an empty array.  
;	It processes corners and edges seperately to calculate edge pixel values.                                                                                                                                                                    
;	
;Parameters:
;   All parameters are not checked, so incorrect parameters will cause a crush of the program.
;   RCX and RDX pointers to arrays are changed while processing the image.
;
;	RCX	- Empty array of bytes to store result values, it has to be the same size as the original array.
;         The array size must equal the array of orgiginal values and hold places for bytes.
;	RDX - Array of bytes with original values of pixels in the format: (BGRBGR...).
;         The array must contain bytes with size not less than 3 times the width of the image.
;	R8	- An integer representing the size of the original array in bytes.
;         Size must be multiple of width of the image, not less than 3 times of width
;	R9	- An integer representing the width of the image in bytes, it equals: width_of_image_in_pixels * 3.
;         The width must be not less than 3, because of the edge processing.
;          
;Used registers:
;   This registers are destroyed.
;	RAX - Used for calculating the new value of the pixel channel.
;	RCX - Pointer to the place to store the result in an array.
;	RDX - Pointer to the original value of the actual processed pixel channel.
;	R10 - Offset pointer, address of the actual processed pixel.
;	R12 - Color chanell counter.
;	R13 - Pointer to the before last pixel in the actual processed line of the image, 
;                                               it is a blue channel of the last pixel.
;   RBX - Negative value of width, used for getting value from the previous line.
;
;Output:
;	This procedure doesn't return any values.
;   The result is stored in a given array.
;	After finishing the procedure, 0 is stored in RAX.
;
;Function looks like:
;void ASMHighPassFilter(
;	unsigned char* bmpDataScan0, = RCX pointer to first byte of picture (left upper corner BLUE) 
;	unsigned char* bmpOriginal,  = RDX array of bytes with original values of pixels (BGR...)
;	int imageSizeInBytes         = R8  (width * height * 3)
;	int width					 = R9  width of the image * 3
;	int height					 = stack [rsp + 40]
;
;Version: 0.1	
;   Simple copying of the image, looping over all pixels and storing it in a new array.
;Version: 0.2
;   Attempting to process corners and edges seperately.
;Version: 0.3
;   Adding a color channel counter.
;Version: 0.4
;   Processing the whole image, but there are overflows in pixel values.
;Version: 0.5
;   Adding handling of edge pixels.
;Version: 0.6
;   Changing comments in the code.
;Version: 0.7
;   Changing the processing of the first and last lines.
;Version: 0.8
;   Adding some comments.
;Version: 0.9
;   Correcting pointers in the last and first lines to original values.
;Version: 1.0
;   Switching to xmm registers, adding all 8 neighborhood values of pixels at once.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;weights values of pixels
.data
    matrix db -1, -1, -1, -1, 9, -1, -1, -1, -1	;3x3 matrix with weights

.code

ASMHighPassFilter proc
	mov r10, 0				                ;reset the offset pointer
	mov r12, 0				                ;reset color chanell counter
	mov r13, r9				                ;pointer to before last pixel in actual line
	sub r13, 3				                ;move pointer to the before last pixel in line
    xor rbx, rbx                            ;reset rbx register
    sub rbx, r9                             ;load negative value of width of the image


FirstPixel:                                 ;processing first pixel - left upper corner
	cmp r10, 3						        ;process all three channels
	je TopPixels					        ;checks if it is third channel and if equals jumps to processing pixels in the first line
    pinsrb xmm1, byte ptr [rdx], 0               ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx], 1               ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + 3], 2           ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx], 3               ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx + 3], 4           ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx + r9], 5          ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 6          ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + r9 + 3], 7      ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r10 	                            ;increment offset 
	inc rdx		                            ;increment pointer in array to actual pixel original value
	inc rcx		                            ;increment pointer in array to actual pixel new value
	jmp FirstPixel	                        ;return to loop

TopPixels:                                  ;process center of first line, except corners  
	cmp r10, r13					        ;check if it is before the last pixel in the first line
	je RightCorner                          ;if it is last pixel process right corner of image
    pinsrb xmm1, byte ptr [rdx - 3], 0           ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx], 1               ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + 3], 2           ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx - 3], 3           ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx + 3], 4           ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx + r9 - 3], 5      ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 6          ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + r9 + 3], 7      ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
	jmp TopPixels	                        ;return to loop

RightCorner:                                ;processing right top corner
	cmp r10, r9                             
	je LeftEdgeReset						;checking if pointer of actual place in line is equal width - 1
    pinsrb xmm1, byte ptr [rdx - 3], 0           ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx], 1               ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx], 2               ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx - 3], 3           ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx], 4               ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx + r9 - 3], 5      ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 6          ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 7      ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
	jmp RightCorner	                        ;return to loop

LeftEdgeReset:                              ;Checking if it is the last line of the image
	mov eax, dword ptr [rsp + 40]           ;getting remaining number of lines
	dec eax                                 ;decrementing remaining number of lines
	cmp eax, 1                              ;checking if last
	je LastLine                             ;jump to processing last line
	mov dword ptr [rsp + 40], eax			;if not last store new remaining number of lines
	mov r12, 0								;reset chanell counter
	add r13, r9								;move pointer of last pixel of line to the next line

LeftEdge:									;Calculate Left edge pixel
	cmp r12, 3                              ;process all channels
	je Center                               ;after that start processing in the middle of image
    pinsrb xmm1, byte ptr [rdx + rbx], 0         ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 1         ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx + 3], 2     ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx], 3               ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx + 3], 4           ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx + r9], 5          ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 6          ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + r9 + 3], 7      ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r12                                 ;increment channel counter                               
	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
	jmp LeftEdge	                        ;return to loop

RightEdgeReset:                             ;Reset channel counter
	mov r12, 0                              
RightEdge:                                  ;Process Right edge pixel
	cmp r12, 3                              
	je LeftEdgeReset                        ;check if all channels are done
    pinsrb xmm1, byte ptr [rdx + rbx - 3], 0     ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 1         ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 2         ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx - 3], 3           ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx], 4               ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx + r9 - 3], 5      ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 6          ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 7          ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r12                                 ;increment channel counter                               
	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
	jmp RightEdge                           ;return to loop

Center:                                     ;process center of line, center of image
    cmp r10, r13
    je RightEdgeReset                       ;checking if it is last pixel in actual line
    pinsrb xmm1, byte ptr [rdx + rbx - 3], 0     ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 1         ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx + 3], 2     ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx - 3], 3           ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx + 3], 4           ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx + r9 - 3], 5      ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx + r9], 6          ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + r9 + 3], 7      ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
    jmp Center                              ;return to loop

LastLine:
	add r13, r9			                    ;move pointer of the end of line to the last pixel
LLFirstPixelReset:
	mov r12, 0                              ;reset color channel counter
LLFirstPixel:
	cmp r12, 3                              
	je BottomPixels                         ;check if all color channels are done
    pinsrb xmm1, byte ptr [rdx + rbx], 0         ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 1         ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx + 3], 2     ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx - 3], 3           ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx + 3], 4           ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx], 5               ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx], 6               ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + 3], 7           ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r12                                 ;increment channel counter                               
	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
	jmp LLFirstPixel                        ;return to loop

BottomPixels:
	cmp r10, r13
	je LastPixelReset
    pinsrb xmm1, byte ptr [rdx + rbx - 3], 0     ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 1         ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx + 3], 2     ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx - 3], 3           ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx + 3], 4           ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx - 3], 5           ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx], 6               ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx + 3], 7           ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0
                             
	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
	jmp BottomPixels                        ;return to loop


LastPixelReset:
	mov r12, 0
;right bottom corner
LastPixel:
	cmp r12, 3
	je TheEnd
    pinsrb xmm1, byte ptr [rdx + rbx - 3], 0     ;load original value of left upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 1         ;load original value of upper pixel
    pinsrb xmm1, byte ptr [rdx + rbx], 2         ;load original value of right upper pixel
    pinsrb xmm1, byte ptr [rdx - 3], 3           ;load original value of left pixel
    movzx  eax , byte ptr [rdx]                  ;load original value of actual pixel
    pinsrb xmm1, byte ptr [rdx], 4               ;load original value of right pixel
    pinsrb xmm1, byte ptr [rdx - 3], 5           ;load original value of left down pixel
    pinsrb xmm1, byte ptr [rdx], 6               ;load original value of down pixel
    pinsrb xmm1, byte ptr [rdx], 7               ;load original value of right down pixel
    movsx edi, matrix[4]                    ;load weight of middle pixel
    imul eax, edi                           ;multiply middle pixel by weight
    pxor xmm2, xmm2							;reset xmm2 register
    psadbw xmm1, xmm2						;adding all values of pixel and store in xmm1
    movd ebx, xmm1							;move result of adding to ebx
    sub eax, ebx							;subtracting sum of values of neigbour pixels from middle pixel value multiplied by weight
    mov ebx, 255						    ;checking for overflow of new of pixel
    cmp eax, ebx						    ;	
    cmovg eax, ebx						    ;if value is greater than 255, it will be set 255
    mov ebx, 0							    ;checking for underflow
    cmp eax, ebx						    ;
    cmovl eax, ebx						    ;if value is less than 0, it will be set 0
	mov byte ptr [rcx], al					;store result byte in bmpdatascan0

	inc r12                                 ;increment channel counter                               
	inc r10			                        ;increment offset pointer
	inc rdx			                        ;increment pointer in array to actual pixel original value
	inc rcx			                        ;increment pointer in new array to store new value of actual pixel
	jmp LastPixel                           ;return to loop


TheEnd:                                     ;the end of processing image
	xor rax, rax                            ;reset accumulator
	ret

ASMHighPassFilter endp
end