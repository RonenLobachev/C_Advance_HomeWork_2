#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "md5.h" //using void md5String(const char* input, char* output);
#include "md4.h" //using void md4String(const char* input, char* output);

#define MD4_HASH 0
#define MD5_HASH 1
#define True 1
#define False 0
typedef unsigned char HashType;
typedef unsigned char Byte;
typedef char Bool;

typedef struct msgWithHash
{
	unsigned int id;
	char* text;
	union
	{
		Byte md4[16];
		Byte md5[16];
	} hashDigest;
	HashType type;
}msgWithHash;

typedef struct msgListNode msgListNode;
struct msgListNode
{
	msgWithHash* data;
	msgListNode* next;
};

void print_hash(const unsigned char* p);
msgWithHash* createMessage(unsigned int id, const char* text, HashType type);
void printMessage(const msgWithHash* message);
msgListNode* addMessage(msgListNode* head, msgWithHash* data);
int findMsgByHashIterative(const msgListNode* head, Byte hash[16]);
int findMsgByHashRecursive(const msgListNode* head, Byte hash[16]);
msgListNode* deleteMessage(msgListNode* head, unsigned int id);
Bool verifyIntegrity(msgListNode* head, unsigned int id, Byte compareHash[16]);
msgListNode* reverseMsgList(msgListNode* head);
void freeMsgList(msgListNode* head);
void print(msgListNode* head);


int main() {
	int nodeHashIndex = 0;
	/******************** Example 1 from PDF ********************************************/
	
	/*const char* sentence = "this is some text that I want to hash.";
	printf("text: \"%s\"\n", sentence);
	unsigned char hash[16] = { 0 };
	md5String(sentence, hash);
	printf("md5: ");
	print_hash(hash);

	printf("md4: ");
	md4String(sentence, hash);
	print_hash(hash);*/
	
	/******************** Example 2 from PDF ********************************************/

	const msgWithHash* msg1 = createMessage(87651233, "", MD4_HASH);
	const msgWithHash* msg2 = createMessage(3, "banana1", MD5_HASH);
	const msgWithHash* msg3 = createMessage(275, "banana2!", MD4_HASH);
	const msgWithHash* msg4 = createMessage(87651231, "Hello World", MD4_HASH);

	printMessage(msg1);
	printMessage(msg2);
	printMessage(msg3);
	printMessage(msg4);

	//add messages to the linked list
	msgListNode* head = NULL;
	head = addMessage(head, msg1);
	head = addMessage(head, msg2);
	head = addMessage(head, msg3);
	head = addMessage(head, msg4);

	//print linked list
	print(head);

     
	nodeHashIndex =findMsgByHashIterative(head, msg4->hashDigest.md4);
	
	if(nodeHashIndex > 0)
	{
		printf("Hash found in index number %d\n", nodeHashIndex);
	}

	else if (nodeHashIndex == -1)
	{
		printf("Hash not found in the linked list\n");
	}

	else
	{
		printf("Error in the hash number\n");
	}
	
	nodeHashIndex = findMsgByHashRecursive(head, msg2->hashDigest.md4);

	if (nodeHashIndex > 0)
	{
		printf("Hash found in index number with recursive function %d\n", nodeHashIndex);
	}
	else if (nodeHashIndex == -1)
	{
		printf("Hash not found in the linked list with recursive function\n");
	}
	else
	{
		printf("Error in the hash number\n");
	}

	

}

void print_hash(const unsigned char* p) {
	for (unsigned int i = 0; i < 16; ++i) {
		printf("%02x", p[i]);
	}
	printf("\n");
}

msgWithHash* createMessage(unsigned int id, const char* text, HashType type) 
{

	msgWithHash *msg = (msgWithHash*) calloc(1,sizeof(msgWithHash));

	if (msg == NULL ) 
	{
		printf("Error: Out of memory");
	}

	msg->id = id;

	msg->type = type;
	msg->text = (char*) malloc((strlen(text) + 1 )* sizeof(char));

	if (msg->text == NULL)
	{
		printf("Error: Out of memory");
	}

	strcpy(msg->text, text);

	if (type == MD4_HASH) 
	{
		md4String(msg->text, msg->hashDigest.md4);
	}
	else if (type == MD5_HASH)
	{
		md5String(msg->text, msg->hashDigest.md5);
	}
	else
	{
		return NULL;
	}


	return msg;


}


void printMessage(const msgWithHash* message)
{

	if (message == NULL)
		return;

	printf("message id : %d\n", message->id);
	printf("message text : %s \n", message->text);

     if (message->type == MD5_HASH)
	{
		 printf("Md5 hash:");
		 print_hash(message->hashDigest.md5);
	}
	else if  (message->type == MD4_HASH)
	{
		printf("Md4 hash:");
		print_hash(message->hashDigest.md4);
	}

	 printf("\n\n");

}

msgListNode* addMessage(msgListNode* head, msgWithHash* data) 
{
	msgListNode *ptr;
	

  	msgListNode *temp = (msgListNode*) calloc(1 , sizeof(msgListNode));
	if (!temp) 
	{
		puts("allocation failed");
		exit(0);
	}

	temp->next = NULL;
	
	temp->data = data;
	

    //add new node in case the list is empty.
	if (head == NULL)
	{		
		head = temp;
	}

	//this section runs if the new val is smaller then
    // the first value in the linked list
    // we simple insert temp at the beginning of the linked list

	else if (temp->data->id <= head->data->id)
	{
		temp->next = head;
		head = temp;

	}
	  else
	{
		// we are iterating each element of the linked list
		// to find the node whose value is just smaller than val
		ptr = head;
		
		while (ptr->next != NULL &&  ptr->next->data->id < temp->data->id)
		{
			ptr = ptr->next;

		}
		// ptr now points to the node whose value is just less than val
		// we simply insert temp between ptr and ptr->next
		temp->next = ptr->next;
		ptr->next = temp;
				
	}
	
	return head;

}


/* Print all the elements in the linked list */
void print(msgListNode* head)
{
	msgListNode* current_node = head;
	while (current_node != NULL) {
		printf("%d \n", current_node->data->id);
		current_node = current_node->next;
	}
}

int findMsgByHashIterative(const msgListNode* head, Byte hash[16]) 
{
	msgListNode *ptr = head;
	int nodeIndex = 1;
	int result;

	if ((hash == NULL) || (head == NULL))
	{
		return -2;
	}

	while (ptr->next != NULL)
	{

		result = memcmp(hash, ptr->data->hashDigest.md4, sizeof(ptr->data->hashDigest.md4));
       
		if (result == 0)
		{
			  return nodeIndex;
		}
			
		ptr = ptr->next;
		nodeIndex++;

	}
	// check if  last value in the linked list have the same hash number
	result = memcmp(hash, ptr->data->hashDigest.md4, sizeof(ptr->data->hashDigest.md4));
	if (result == 0)
	{
		return nodeIndex;
	}

	return -1;	
}


int findMsgByHashRecursive(const msgListNode* head, Byte hash[16]) 
{
	static counter = 1;

	if ((hash == NULL) || (head == NULL))
	{
		return -2;
	}

	if (head == NULL) 
	{
		return  -1;
	}


      int result=memcmp(hash,head->data->hashDigest.md4, sizeof(head->data->hashDigest.md4));
	  if (result == 0) 
	  {
		  return counter;
	  }

	  counter++;

	  return findMsgByHashRecursive(head->next, hash);

}