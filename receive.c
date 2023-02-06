
/*

File receive.c


Purpose: simulate the receiveing end of a message over the network.

The program converts a message encodded as an array of short integers into characters.
The input message uses 1-bit error correction code.
As part of the simulation, the program also corrects any 1-bit errors in the receiving message.



*/


/************************************************************************/

// INCLUDE FILES

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "bit_manipulation.h"


/************************************************************************/
//MACROS  / DEFINES



#define XSTR(A) STR(A)
#define STR(A) #A
#define MAX_INPUT_LENGTH 8192
#define MAX_MSG_LENGTH 2048
#define P1_MASK 0xaa8   // 0b0101010101000
#define P2_MASK 0xcc8	// 0b0110011001000
#define P4_MASK 0x10e0	// 0b1000011100000
#define P8_MASK 0x1e00  // 0b1111000000000


/************************************************************************/
// FUNCTION PROTOTYPES


void short2Char(short encodedChar, char *c);
void correctCode(short *num);
void readEncodedMessage(short *buf, int len, int *numRead);




/**************************************************************************/

int main(int argc, char *argv[])

{
	int rc = 0;		// return code

	char s[MAX_MSG_LENGTH] = { '\0' };  // message
	short encodedMsg[MAX_MSG_LENGTH];
	int numRead = 0;  // number of characters in the message
	int i;

	// read the message
	printf("Enter the transmitted message: ");
	readEncodedMessage(encodedMsg, MAX_MSG_LENGTH, &numRead);



	// print the corrected message
	printf("\n\n Transmitted Message:\n");
	for (i = 0; i < numRead; i++) {
		char c;
		short2Char(encodedMsg[i], &c);
		printf("%c", c);
    }
    printf("\n");

    // correct the message
	for (i = 0; i < numRead; i++) {
		correctCode(&encodedMsg[i]);
	}



	// print the corrected message
	printf("\n\n Corrected Transmitted Message:\n");
	for (i = 0; i < numRead; i++) {
		char c;
		short2Char(encodedMsg[i], &c);
		printf("%c", c);
	}
	printf("\n");

	return(0);
}


/***********************************************************************************/
/* reads a message from the user

input
len - maximum length of mesage that should be read

output
receivedMsg - contains the message
numRead - the number of characters in the message

Asumption:
function does not do any error checking of the input.  It expects to receive
a sequence of positive short integers.

*/


void readEncodedMessage(short *receivedMsg, int len, int *numRead)
{
	int i;
	int rc;
	char s[MAX_INPUT_LENGTH+1];
	char *token = NULL;
	*numRead = 0;

	s[MAX_INPUT_LENGTH] = '\0';
	scanf("%"XSTR(MAX_INPUT_LENGTH)"[^\n]s", s);
	token = strtok(s, " ");

	for (i = 0; token != NULL  &&  i < len;token = strtok(NULL, " "), i++) {
		rc = sscanf(token, "%hd", &receivedMsg[i]);
	}

	*numRead = i; // set the number of read integers

    // empty the input buffer
    for( ; getchar() != '\n'; );


}




/*************************************************************************/
/* assign bits 3,5,6,7,9,10,11,12 of a integer to bits 0-7 of a char

input
encodedNum - a short integer

output
*c - the modified char that is embedded in encodedNum

return:
none

comment:
this is the "inverse" of the function char2Short in the transmit program

*/


void short2Char(short encodedNum, char *c)

{
	int i;			//counter for the for loop/ keep track of which bit currently on
	int bitSet;		//result if the current bit should be set or not


	*c = 0;			// initialize *c

	for (i = 0; i < 8; i++) {

		//check to see if the 1st bit in the char should be set based off if the 3rd bit in the short is set
		if(i == 0){
			bitSet = isShortBitSet(encodedNum,3);

		//check to see if the 2nd bit in the char should be set based off if the 5th bit in the short is set
		}else if( i == 1){
			bitSet = isShortBitSet(encodedNum,5);

		//check to see if the 3rd bit in the char should be set based off if the 6th bit in the short is set
		}else if( i == 2){
			bitSet = isShortBitSet(encodedNum,6);

		//check to see if the 4th bit in the char should be set based off if the 7th bit in the short is set
		}else if( i == 3){
			bitSet = isShortBitSet(encodedNum,7);

		//check to see if the 5th bit in the char should be set based off if the 9th bit in the short is set
		}else if( i == 4){
			bitSet = isShortBitSet(encodedNum,9);

		//check to see if the 6th bit in the char should be set based off if the 10th bit in the short is set
		}else if( i == 5){
			bitSet = isShortBitSet(encodedNum,10);

		//check to see if the 7th bit in the char should be set based off if the 11th bit in the short is set
		}else if( i == 6){
			bitSet = isShortBitSet(encodedNum,11);

		//check to see if the 8th bit in the char should be set based off if the 12th bit in the short is set
		}else if( i == 7){
			bitSet = isShortBitSet(encodedNum,12);
		}

		//if the bit in the char whould be set, set it
		if(bitSet == 1){
			setCharBit(i, c);
		}
	}
}


/*************************************************************************************/


/* purpose:
performs error corrrections, if needed on the short integer

The function checks the parity bits for error and see if thre is a discrepancy between
the transmitted parity bits and the computed parity bits for the received number.
If there is a decrepancy then it finds the bit number that was flipped.

If there is an error then it fixes it.

input
num - the address of the short number

output:
*num - the modified number
*/


void correctCode(short *num)

{
	short mask; 		//the mask for each parity bit
	int sum;			//the sum of bits set to 1
	short result;		//the addition of mask and num
	int bitNumber = 0; 	//bit number with the error bit
	int storedParity; 	//the parity bit in the encoded char




	//Parity 1--------------------------------------------------------------------
	//Store parity bit 1 for checking later
	storedParity = isShortBitSet(*num, 1);

	//look at the corresponding bits for parity 1
	mask = P1_MASK;
	result = mask & *num;

	//count how many corresponding bits are set to 1
	sum = countBits(result);

	//if the sum is even and the parity is set, add parity number to bitNumber variable
	if((sum % 2) != storedParity){
		bitNumber += 1;
	}


	//Parity 2--------------------------------------------------------------------
	//Store parity bit 2 for checking later
	storedParity = isShortBitSet(*num, 2);

	//look at the corresponding bits for parity 2
	mask = P2_MASK;
	result = mask & *num;

	//count how many corresponding bits are set to 1
	sum = countBits(result);

	//if the sum is even and the parity is set, add parity number to bitNumber variable
	if((sum % 2) != storedParity){
		bitNumber += 2;
	}


	//Parity 4--------------------------------------------------------------------
	//Store parity bit 4 for checking later
	storedParity = isShortBitSet(*num, 4);

	//look at the corresponding bits for parity 4
	mask = P4_MASK;
	result = mask & *num;

	//count how many corresponding bits are set to 1
	sum = countBits(result);

	//if the sum is even and the parity is set, add parity number to bitNumber variable
	if((sum % 2) != storedParity){
		bitNumber += 4;
	}


	//Parity 8--------------------------------------------------------------------
	//Store parity bit 8 for checking later
	storedParity = isShortBitSet(*num, 8);

	//look at the corresponding bits for parity 8
	mask = P8_MASK;
	result = mask & *num;

	//count how many corresponding bits are set to 1
	sum = countBits(result);

	//if the sum is even and the parity is set, add parity number to bitNumber variable
	if((sum % 2) != storedParity){
			bitNumber += 8;
	}


	//---------------------------------------------------------------------------
	//previous steps calculate which bit is problematic
	//if there is a mistake, flip the problematic bit to fix errors
	if(bitNumber != 0){
		flipBitShort(bitNumber,num);
	}
}
