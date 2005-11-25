#
# This script was written by Renaud Deraison <deraison@cvs.nessus.org>
#
# See the Nessus Scripts License for details
#

if(description)
{
 script_id(10234);
 script_version ("$Revision: 1.11 $");
 script_cve_id("CAN-1999-0613");
 name["english"] = "sprayd service";
 name["francais"] = "Service sprayd";
 script_name(english:name["english"], francais:name["francais"]);
 
 desc["english"] = "
The sprayd RPC service is running. 
You should disable this service, as it may be used to saturate 
your network.
Furthermore, it might become a security threat in the future, 
if a RPC vulnerability is discovered.

Risk factor : Low";


 desc["francais"] = "
Le service RPC sprayd est pr�sent.
Vous devriez d�sactiver ce service, car il peut �tre utilis�
pour saturer le r�seau.
De plus, il pourrait devenir un jour une faille de 
s�curit� si une vulnerabilit� RPC est d�couverte.

Facteur de risque : Faible";


 script_description(english:desc["english"], francais:desc["francais"]);
 
 summary["english"] = "Checks the presence of a RPC service";
 summary["francais"] = "V�rifie la pr�sence d'un service RPC";
 script_summary(english:summary["english"], francais:summary["francais"]);
 
 script_category(ACT_GATHER_INFO);
 
 
 script_copyright(english:"This script is Copyright (C) 1999 Renaud Deraison",
		francais:"Ce script est Copyright (C) 1999 Renaud Deraison");
 family["english"] = "RPC"; 
 family["francais"] = "RPC";
 script_family(english:family["english"], francais:family["francais"]);
 script_dependencie("rpc_portmap.nasl");
 script_require_keys("rpc/portmap");
 exit(0);
}

#
# The script code starts here
#

include("misc_func.inc");



RPC_PROG = 100012;
tcp = 0;
port = get_rpc_port(program:RPC_PROG, protocol:IPPROTO_UDP);
if(!port){
	port = get_rpc_port(program:RPC_PROG, protocol:IPPROTO_TCP);
	tcp = 1;
	}

if(port)
{
 if(tcp)security_warning(port);
 else security_warning(port, protocol:"udp");
}
