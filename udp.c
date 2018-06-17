#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<errno.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<netinet/udp.h>

//IPPROTO_RAW

int main (void)
{
    int s = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);//utworzenie gniazda
    int liczba_Pakietow;
    char adres_zrodlowy[15],adres_docelowy[15];
    if(s == -1)
    {
        perror("Blad utworzenia gniazda");
        exit(1);
    }

    char datagram[4096] , source_ip[32] , *data;

    memset (datagram, 0, 4096);//wypenianie pamieci (adres pocz, wartosc, liczba bajtow)

    struct iphdr *iph = (struct iphdr *) datagram;
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in sin;

    data = datagram + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data, "abcdefghijk");

    printf( "Podaj adres zrodlowy: ");
    scanf("%14s",adres_zrodlowy);
    printf( "Podaj adres docelowy: ");
    scanf("%14s",adres_docelowy);
    strcpy(source_ip , adres_zrodlowy);
    sin.sin_family = AF_INET; // rodzin adresow
    sin.sin_addr.s_addr = inet_addr(adres_docelowy); // numer portu
    sin.sin_port = htons(80);


    //IP
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + strlen(data);
    iph->id = htonl (54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    iph->saddr = inet_addr ( source_ip );
    iph->daddr = sin.sin_addr.s_addr;


    //Naglowek UDP
    udph->source = htons (4950);
    udph->dest = htons (80);
    udph->check = 0;
    udph->len = htons(8 + strlen(data));


    int j = 1;
    const int *optval = &j;

    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, optval, sizeof (j)) < 0) //obsluga gniazda - (deskryptor gniazda,
    	//poziom, opcje protokolow(udostepnienie naglowka IP) ,rozmiar zmiennej wskazywanej przez optval)
    {
        printf("Blad udostepnienia naglowka. Blad numer: %d . Tresc bledu: %s \n" , errno , strerror(errno));

    }
    		printf( "Liczba pakietow do wyslania: ");
    		scanf("%d", &liczba_Pakietow);

    		int i;
    	    i = 0;

    	    while ( i < liczba_Pakietow)
    	    {
    	    	i++;

    	        if (sendto (s, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
    	        {
    	            perror("Wysylanie pakietow zakonczone niepowodzeniem");
    	        }
    	        else
    	        {
    	            printf ("%d. Pakiet zostal wyslany z powodzeniem. Wyslane dane: %s \n" ,i,data);
    	        }
    	    }


    return 0;
}

