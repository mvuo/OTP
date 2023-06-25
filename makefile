build:
	clear
	gcc -g -Wall -std=c99 enc_server.c utils.c -o enc_server
	gcc -g -Wall -std=c99 dec_server.c utils.c -o dec_server
	gcc -g -Wall -std=c99 enc_client.c utils.c -o enc_client
	gcc -g -Wall -std=c99 dec_client.c utils.c -o dec_client
	gcc -g -Wall -std=c99 keygen.c utils.c -o keygen


clean:
	rm -f enc_server
	rm -f dec_server
	rm -f enc_client
	rm -f dec_client
	rm -f keygen

