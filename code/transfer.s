;				AREA    MyData, DATA, READWRITE, align=3
;Move			SPACE 	4
				
				AREA Transfer,CODE,READONLY
				EXPORT transfer
transfer		PROC
				MOV R12,sp
				STMFD sp!,{r1-r12,lr}
				;R0 player
				;R1 moveWall
				;R2 direction
				;R3 y
				;R4 x
				MOV R6,#0 ;REGISTRO RISULTATO
				LDR   r4, [r12] ;5° parametro
				LSL R6, R4, #0  ; Spostiamo i primi 8 bit di r5 in r6
				
				LSL R7, R3, #8  ; I successivi 8 bit di r4 in r7
				ORR R6, R6, R7  ; Combiniamo i primi 8 bit con i successivi 8 bit in r6
				
				LSL R7, R2, #16     ; Spostiamo i successivi 8 bit di r3 in r7
				ORR R6, R6, R7      ; Combiniamo i primi 16 bit con i successivi 8 bit in r4
				
				LSL R7, R1, #20     ; Spostiamo i successivi 4 bit di r2 in r7
				ORR R6, R6, R7      ; Combiniamo i primi 16 bit con i successivi 8 bit in r4
				
				LSL R7, R0, #24     ; Spostiamo i successivi 4 bit di r2 in r7
				ORR R6, R6, R7      ; Combiniamo i primi 16 bit con i successivi 8 bit in r4
				
				; setup a value for R0 to return
				MOV	  r0, r6
				
				LDMFD sp!,{r1-r12,pc}
				ENDP
				END