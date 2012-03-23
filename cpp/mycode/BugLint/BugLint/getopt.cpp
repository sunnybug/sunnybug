#include <stdio.h>
#include "getopt.h"

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#define stricmp strcasecmp
#endif

#include <vector>

/*
泽黻鲨?镳孱狃噻钼囗? 矬蝈??羿殡囔
*/
static void absToRelPath(std::string spath, std::string &tpath)
{
	int i,j,k,s;
	char *div;
	if(!spath.length()) return;
	if(spath[0]=='/')
	{
		// Unix
		i=1;
		s=0;
		div="/";
	} else
		if(spath[1]==':') // Windows
		{
			if(tolower(spath[0])!=tolower(tpath[0])) // 朽珥 滂耜?
				return;
			i=3;
			s=2;
			div="\\";
		}
		while(1)
		{
			j=(int)spath.find_first_of(div, i);
			k=(int)tpath.find_first_of(div, i);
			if(j==-1)
			{
				tpath=tpath.substr(i);
				return;
			}
			if(j==k && ((div[0]=='\\' && !stricmp(spath.substr(i, j-i).c_str(), tpath.substr(i, k-i).c_str())) || (div[0]=='/' && spath.substr(i, j-i)==tpath.substr(i, k-i))))
			{
				i=j+1;
				continue;
			}
			if(spath.find_first_of(div, j+1)==-1)
			{
				tpath=".."+(div+tpath.substr(i));
			} else
				tpath=tpath.substr(s);
			return;
		}
}

static void relToAbsPath(std::string spath, std::string &tpath)
{
	char *div;
	int s;
	if(spath[0]=='/')
	{
		// Unix
		if(tpath[0]!='/') tpath=spath.substr(0, spath.find_last_of("/")+1)+tpath;
		div="/";
		s=0;
	} else
		if(spath[1]==':')
		{
			// Windows
			if(tpath[1]!=':')
			{
				if(tpath[0]!='\\')
				{
					tpath=spath.substr(0, spath.find_last_of("\\")+1)+tpath;
				} else
				{
					tpath=spath.substr(0, 2)+tpath;
				}
			}
			div="\\";
			s=2;
		}
		int i,j;
		if(div[0]=='\\')
			for(i=s;i<(int)tpath.length();i++)
				if(tpath[i]=='/') tpath[i]='\\';
		for(i=s;i<(int)tpath.length();)
		{
			if(tpath.substr(i, 4)==std::string(div)+".."+div)
			{
				if(i>s)
					j=(int)tpath.find_last_of(div, i-1);
				else
					j=s;
				tpath=tpath.substr(0, j)+tpath.substr(i+3);
				i=j;
			} else
				if(tpath.substr(i, 3)==std::string(div)+"."+div)
				{
					tpath=tpath.substr(0, i)+tpath.substr(i+2);
				}
				else
					if(tpath.substr(i, 2)==std::string(div)+div)
					{
						tpath=tpath.substr(0, i)+tpath.substr(i+1);
					}
					else i++;
		}
}

using namespace std;

static char *parammessages[]=
{
	// 杨钺龛 钺 铠栳赅?
	"No error",
	"Parameter name length exceed",
	"Parameter name cannot contain symbols '=', '[', ']' or whitespaces",
	"Parameter already exists",
	"Abbreviation already exists",
	"Type already exists",
	"%s: invalid parameter",
	"Missing required parameter %s",
	"%s: parameter argument missing",
	"Syntax error",
	"Missing enclosing quotemark",
	"%s: invalid parameter type",
	"Missing enclosing bracket",
	"Invalid file signature",
	"%s: unable to open file",
	// 酿筱桢 耦钺龛
	"required parameter"
};

/*
ParamContainer::TList class implementation
*/
ParamContainer::TList::TList():tlist(new std::map<std::string, ParamContainer>)
{
}

ParamContainer::TList::~TList()
{
	delete tlist;
}

ParamContainer::TList::TList(const TList &c)
:tlist(new std::map<std::string, ParamContainer>(*c.tlist))
{
}

ParamContainer::TList & ParamContainer::TList::operator =(const ParamContainer::TList &c)
{
	if(&c==this) return *this;
	delete tlist;
	tlist=new std::map<std::string, ParamContainer>(*c.tlist);
	return *this;
}

ParamContainer & ParamContainer::TList::operator[](std::string key)
{
	return (*tlist)[key];
}

/*
绣嚯桤圉? 觌囫襦 ParamContainer
*/

// 锑犟桁嚯? 潆桧?桁屙?镟疣戾蝠?
const int ParamContainer::maxpnamelength=25;
// 袜犷?耦钺龛?镱 箪铍鬣龛?
char **ParamContainer::messages=parammessages;

ParamContainer::param ParamContainer::dumbparam=ParamContainer::param();

/*
念徉怆屙桢 眍忸泐 忄腓漤钽?镟疣戾蝠??耧桉铌
*/
ParamContainer::errcode ParamContainer::addParam(
	std::string pname, char abbr, int type,
	std::string helptopic, std::string defvalue, std::string allowedtypes)
{
	if(pname.length()>maxpnamelength) return errParameterNameLengthExceed;
	// 谚焘铍?= [] 礤 祛泱?怦蝠鬻囹 ?桁屙?镟疣戾蝠?
	if(pname.find_first_of("= []\t")!=-1) return errParameterNameInvalid;
	if(plist.find(pname)!=plist.end()) return errDublicateParameter;
	if(abbr && alist.find(abbr)!=alist.end()) return errDublicateAbbreviation;
	param p;
	p.value=p.defvalue=defvalue;
	p.help=helptopic;
	p.pflag=type;
	p.abbr=abbr;
	p.wasset=false;
	p.allowedtypes=allowedtypes;
	plist[pname]=p;
	if(abbr) alist[abbr]=pname;
	vlist.push_back(pname);
	return errOk;
}

/*
念徉怆屙桢 眍忸泐 耦耱噔眍泐 蜩镟 镟疣戾蝠钼
*/
ParamContainer::errcode ParamContainer::addParamType(std::string tname, const ParamContainer &p)
{
	if(tname.find_first_of("= []\t")!=-1) return errParameterNameInvalid;
	if(tlist->find(tname)!=tlist->end()) return errDublicateType;
	tlist[tname]=p;
	tindex.push_back(tname);
	return errOk;
}


ParamContainer::errcode ParamContainer::delParam(std::string pname)
{
	std::map<std::string, param>::iterator it=plist.find(pname);
	if(it!=plist.end()) return errInvalidParameter;
	plist.erase(it);
	return errOk;
}

/*
厌痤?珥圜屙栝 镟疣戾蝠钼
*/
void ParamContainer::reset(void)
{
	// 闰屦囹铕 镱 耧桉牦 镟疣戾蝠钼
	map<string, param>::iterator it;
	for(it=plist.begin(); it!=plist.end(); it++)
	{
		it->second.value=it->second.defvalue;
		it->second.wasset=false;
		if(it->second.p)
		{
			delete it->second.p;
			it->second.p=NULL;
		}
	}
}

ParamContainer::errcode ParamContainer::parseCommandLine(std::string cmdline)
{
	lastcmdline=cmdline;
	errcode err=lexicalAnalysis(cmdline);
	if(err!=errOk) return err;
	return syntaxAndSemanticsAnalysis();
}

ParamContainer::errcode ParamContainer::parseCommandLine(int argc, char *argv[])
{
	std::string s;
	for(int i=1; i<argc; i++)
		s=s+argv[i]+" ";
	return parseCommandLine(s);
}

// 隋犟梓羼觇?囗嚯桤 觐爨礓眍?耱痤觇 桦?羿殡?镳铄牝?
ParamContainer::errcode ParamContainer::lexicalAnalysis(std::string s)
{
	// 羊囹篑 囗嚯桤囹铕?
	enum {
		normal,			// 体驿?镟疣戾蝠囔?
		oneminus,		// 项耠?钿眍泐 扈眢襦
		value,			// 马篁痂 蝈犟蝾忸?耱痤麝?
		quotedvalue,	// 马篁痂 耱痤麝??赅恹麝圊
	} state=normal;
	// 诣?脲犟梓羼觐?邃桧桷?
	cmdlineel::elflag flag;
	// 羊囵蝾忄 镱玷鲨 珥圜屙? 蝈牦?脲犟梓羼觐?邃桧桷?
	int startpos, pos=0;
	std::string valpart="";
	// 西桉蜿?蜞犭桷?脲犟梓羼觐?疋葛蜿?
	lexconv.clear();
	// 橡钽铐 镱 耱痤赍
	int i;
	for(i=0; i<(int)s.length(); i++)
	{
		switch(state)
		{
		case normal:
			if(strchr("\t ", s[i])) continue;
			if(s[i]=='"')
			{
				flag=cmdlineel::value;
				state=quotedvalue;
				startpos=i+1;
				pos=i;
				continue;
			}
			if(s[i]=='\n')	// \n = 镳钺咫, 扈眢? 扈眢?
			{
				flag=cmdlineel::param;
				state=value;
				startpos=i+1;
				pos=i+1;
				continue;
			}
			if(s[i]=='-')
			{
				state=oneminus;
				pos=i;
				continue;
			}
			if(s[i]=='[')
			{
				lexconv.push_back(cmdlineel(cmdlineel::leftbracket, i));
				continue;
			}
			if(s[i]==']')
			{
				lexconv.push_back(cmdlineel(cmdlineel::rightbracket, i));
				continue;
			}
			if(s[i]=='=')
			{
				lexconv.push_back(cmdlineel(cmdlineel::equal, i));
				continue;
			}
			flag=cmdlineel::value;
			state=value;
			pos=startpos=i--;
			continue;
		case value:
			if(strchr("\t []=\n", s[i]))
			{
				state=normal;
				valpart+=s.substr(startpos, i-startpos);
				i--;
				lexconv.push_back(cmdlineel(flag, pos, valpart));
				valpart="";
				continue;
			}
			if(s[i]=='"')
			{
				state=quotedvalue;
				valpart+=s.substr(startpos, i-startpos);
				startpos=i+1;
				continue;
			}
			continue;
		case oneminus:
			if(strchr("\t []=\n", s[i]))
			{
				errpos=i;
				return errInvalidSyntax;
			}
			if(s[i]=='-')
			{
				flag=cmdlineel::param;
				state=value;
				startpos=i+1;
				continue;
			}
			flag=cmdlineel::abbrparam;
			state=value;
			startpos=i--;
			continue;
		case quotedvalue:
			if(s[i]=='"')
			{
				state=value;
				valpart+=s.substr(startpos, i-startpos);
				startpos=i+1;
				continue;
			}
			continue;
		}
	}
	switch(state)
	{
	case quotedvalue:
		errpos=startpos;
		return errMissingEnclosingQuotemark;
	case oneminus:
		errpos=i;
		return errInvalidSyntax;
	case value:
		valpart+=s.substr(startpos, i-startpos);
		lexconv.push_back(cmdlineel(flag, pos, valpart));
	}
	lexconv.push_back(cmdlineel(cmdlineel::eol, (int)lastcmdline.length()));
	return errOk;
}

// 谚眚嚓耔麇耜栝 ?皴爨眚梓羼觇?囗嚯桤 觐爨礓眍?耱痤觇
ParamContainer::errcode ParamContainer::syntaxAndSemanticsAnalysis(bool fromproject)
{
	// 厌痤?耧桉赅 镟疣戾蝠钼
	reset();
	int nonameidx=0;
	int j;
	errcode err;
	char tmp[20];
	for(int i=0; i<(int)lexconv.size(); i++)
	{
		switch(lexconv[i].flag)
		{
		case cmdlineel::value:	// 吾睇?镟疣戾蝠 (礤 ?'-')
			for(;nonameidx<(int)vlist.size() && !(plist[vlist[nonameidx]].pflag & noname);nonameidx++);
			if(nonameidx>=(int)vlist.size())
			{
				errinfo=lexconv[i].val;
				errpos=lexconv[i].pos;
				return errInvalidParameter;
			}
			err=parseComplexType(i, vlist[nonameidx], lexconv[i].val);
			if(err!=errOk) return err;
			nonameidx++;
			break;
		case cmdlineel::param:	// 碾桧睇?镟疣戾蝠 (?'--')
			if(plist.find(lexconv[i].val)==plist.end()) // 湾桤忮耱睇?镟疣戾蝠
			{
				if(allowunknownparams)
				{
					if(lexconv[i+1].flag==cmdlineel::equal)
					{
						i++;
						if(lexconv[i+1].flag==cmdlineel::value) i++;
					}
					break;
				}
				errinfo="--"+lexconv[i].val;
				errpos=lexconv[i].pos;
				return errInvalidParameter;
			}
			if(!fromproject && (plist[lexconv[i].val].pflag & novalue)) // ?镟疣戾蝠?礤 漕腈眍 猁螯 珥圜屙?
			{
				sprintf(tmp, "%04d0000", i);
				plist[lexconv[i].val].value=tmp;
				plist[lexconv[i].val].wasset=true;
				break;
			}
			if(i+1>=(int)lexconv.size() || 
				lexconv[i+1].flag!=cmdlineel::equal) // ?镟疣戾蝠?礤 耱铊?珥圜屙桢, 躅? 漕腈眍 猁螯
			{
				errinfo="--"+lexconv[i].val;
				errpos=lexconv[i].pos;
				return errParameterArgumentMissing;
			}
			i++;
			if(i+1>=(int)lexconv.size() ||
				lexconv[i+1].flag!=cmdlineel::value) // ?镟疣戾蝠?矬耱铄 珥圜屙桢
			{
				err=parseComplexType(i, lexconv[i-1].val, "");
			} else
			{
				// 软圜?镳桉忄桠噱?镟疣戾蝠?珥圜屙桢
				i++;
				err=parseComplexType(i, lexconv[i-2].val, lexconv[i].val);
			}
			if(err!=errOk) return err;
			break;
		case cmdlineel::abbrparam:	// 暑痤蜿栝 镟疣戾蝠 桦?鲥镱麝?镟疣戾蝠钼
			for(j=0; j<(int)lexconv[i].val.length(); j++)
			{
				char c=lexconv[i].val[j];
				// 湾桤忮耱睇?镟疣戾蝠
				if(alist.find(c)==alist.end())
				{
					if(allowunknownparams) break;
					sprintf(tmp, "-%c", c);
					errinfo=string(tmp);
					errpos=lexconv[i].pos+j+1;
					return errInvalidParameter;
				}
				string name=alist[c];
				plist[name].wasset=true;
				if(plist[name].pflag & novalue)
				{
					// ?镟疣戾蝠?礤 漕腈眍 猁螯 珥圜屙?:
					// 镳桉忄桠噱?桎屙蜩翳赅蝾??镥疱躅滂??耠邃簋?狍赈??鲥镱麝?
					sprintf(tmp, "%04d%04d", i, j);
					plist[name].value=tmp;
					continue;
				}
				// 琼圜屙桢 磬镨襦眍 疣玟咫??镟疣戾蝠铎
				if(j+1==lexconv[i].val.length())
				{
					if(lexconv[i+1].flag!=cmdlineel::value) // ?镟疣戾蝠?礤 耱铊?珥圜屙桢, 躅? 漕腈眍 猁螯
					{
						errinfo="--";errinfo[1]=c;
						errpos=lexconv[i].pos+j+1;
						return errParameterArgumentMissing;
					}
					i++;
					err=parseComplexType(i, name, lexconv[i].val);
				} else
					// 琼圜屙桢 磬镨襦眍 耠栩眍 ?镟疣戾蝠铎
					err=parseComplexType(i, name, lexconv[i].val.substr(j+1));
				if(err!=errOk) return err;
				break;
			}
			break;
		case cmdlineel::eol:
			break;
		default:
			errpos=lexconv[i].pos;
			return errInvalidSyntax;
		}
	}
	// 橡钼屦赅, 怦?腓 礤钺躅滂禧?镟疣戾蝠?猁腓 镳桉忸屙?
	map<string, param>::iterator it;
	for(it=plist.begin(); it!=plist.end(); it++)
	{
		if((it->second.pflag & required) && !it->second.wasset)
		{
			if(it->second.pflag & noname)
				errinfo=it->first;
			else
				errinfo="--"+it->first;
			errpos=lexconv[lexconv.size()-1].pos;
			return errRequiredParameterMissing;
		}
		if(it->second.allowedtypes!="" && !it->second.wasset)
		{
			int i=0;
			err=parseComplexType(i, it->first, it->second.value);
			if(err!=errOk) return err;
		}
	}
	return errOk;
}

std::string ParamContainer::getErrCmdLine(int &pos, int maxlength) const
{
	if((int)lastcmdline.length()<=maxlength)
	{
		pos=errpos;
		return lastcmdline;
	}
	if(errpos<maxlength*2/3)
	{
		pos=errpos;
		return lastcmdline.substr(0, maxlength-3)+"...";
	}
	if((int)lastcmdline.length()-errpos<maxlength*2/3)
	{
		pos=maxlength+errpos-(int)lastcmdline.length();
		return "..."+lastcmdline.substr(lastcmdline.length()-maxlength+3);
	}
	pos=maxlength/2;
	return "..."+lastcmdline.substr(errpos-maxlength/2+3, maxlength-6)+"...";
}

// 朽玑铕 耦耱噔眍泐 蜩镟
ParamContainer::errcode ParamContainer::parseComplexType(int &i, std::string name, std::string value)
{
	plist[name].wasset=true;
	if(plist[name].allowedtypes=="")
	{
		// 湾 耦耱噔眍?蜩?
		plist[name].value=value;
		return errOk;
	}
	int pos=i+2, end=i+2, numbrackets=1;
	if(i+1<(int)lexconv.size() && lexconv[i+1].flag==cmdlineel::leftbracket)
	{
		for(i+=2; numbrackets; i++)
		{
			if(i==lexconv.size())
			{
				errpos=lexconv[pos-1].pos;
				return errMissingEnclosingBracket;
			}
			if(lexconv[i].flag==cmdlineel::leftbracket) numbrackets++;
			if(lexconv[i].flag==cmdlineel::rightbracket) numbrackets--;
		}
		end=--i;
	}
	// 湾漕矬耱桁 蜩?
	string s="|"+plist[name].allowedtypes+"|";
	if(s.find("|"+value+"|")==-1)
	{
		errinfo=value;
		errpos=lexconv[pos-2].pos;
		return errInvalidType;
	}
	// 湾耋耱怏栝 蜩?(赅?驽 铐 铌噻嚯? 漕矬耱桁?..)
	if(tlist->find(value)==tlist->end())
	{
		errinfo=value;
		errpos=lexconv[pos-2].pos;
		return errInvalidType;
	}
	plist[name].value=value;
	plist[name].p=new ParamContainer(tlist[value]);
	int j;
	for(j=pos; j<end; j++)
		plist[name].p->lexconv.push_back(lexconv[j]);
	plist[name].p->lexconv.push_back(cmdlineel(cmdlineel::eol, lexconv[pos==end?j-1:j].pos));
	errcode err=plist[name].p->syntaxAndSemanticsAnalysis();
	if(err!=errOk)
	{
		errinfo=plist[name].p->errinfo;
		errpos=plist[name].p->errpos;
	}
	return err;
}

// 骂耨蜞眍忤螯 觐爨礓眢?耱痤牦
std::string ParamContainer::getCmdLine()
{
	int minnovalue=-1, curmin=-1;
	std::string s;
	map<string, param>::iterator it, it2;
	while(1)
	{
		for(it=plist.begin(); it!=plist.end(); it++)
		{
			if((it->second.pflag & novalue) && it->second.wasset)
			{
				int val=atoi(it->second.value.c_str());
				if(val>minnovalue && (curmin==minnovalue || val<curmin))
				{
					curmin=val;
					it2=it;
				}
			}
		}
		if(curmin==minnovalue) break;
		minnovalue=curmin;
		s+="--"+it2->first+" ";
	}
	for(int i=0;i<(int)vlist.size();i++)
	{
		if(!(plist[vlist[i]].pflag & novalue) && plist[vlist[i]].wasset)
		{
			if(!(plist[vlist[i]].pflag & noname))
				s+="--"+vlist[i]+"=";
			if(plist[vlist[i]].p) s+=plist[vlist[i]].value+"["+plist[vlist[i]].p->getCmdLine()+"] ";
			else s+="\""+plist[vlist[i]].value+"\" ";
		}
	}
	s=s.substr(0, s.length()-1);
	return s;
}

ParamContainer::errcode ParamContainer::unsetParam(std::string pname)
{
	map<string, param>::iterator it=plist.find(pname);
	if(it==plist.end()) return errInvalidParameter;
	it->second.value=it->second.defvalue;
	it->second.wasset=false;
	if(it->second.p)
	{
		delete it->second.p;
		it->second.p=NULL;
	}
	return errOk;
}

/*
蔓忸?蝈犟蜞 耧疣怅?镱 镟疣戾蝠?
*/
void ParamContainer::printHelpTopic(FILE *f, std::string topic, int indent, int width, bool linebreak)
{
	if(!width)
	{
		fprintf(f, "%s%s", topic.c_str(), linebreak?"\n":"");
		return;
	}
	width-=indent;
	string s;
	char formatstr[10];
	sprintf(formatstr, "%%s\n%%%ds", indent);
	for(;;)
	{
		if((int)topic.length()<width)
		{
			fprintf(f, "%s%s", topic.c_str(), linebreak?"\n":"");
			return;
		}
		s=topic.substr(0, width);
		int pos = (int)s.rfind(" ");
		if(pos==-1) pos=width;
		s=s.substr(0, pos);
		topic=topic.substr(pos+1);
		fprintf(f, formatstr, s.c_str(), "");
	}
}

void ParamContainer::printHelpTopic( std::ostream& strm, std::string topic, int indent, int width, bool linebreak/*=true*/ )
{
	if(!width)
	{
		char buf[512] = {0};
		_snprintf(buf, sizeof(buf)-1, "%s%s", topic.c_str(), linebreak?"\n":"");
		strm << buf;
		return;
	}
	width-=indent;
	string s;
	char formatstr[10];
	sprintf(formatstr, "%%s\n%%%ds", indent);

	char buf[512] = {0};
	for(;;)
	{
		if((int)topic.length()<width)
		{
			char buf[512] = {0};
			_snprintf(buf, sizeof(buf)-1, "%s%s", topic.c_str(), linebreak?"\n":"");
			strm << buf;
			return;
		}
		s=topic.substr(0, width);
		int pos = (int)s.rfind(" ");
		if(pos==-1) pos=width;
		s=s.substr(0, pos);
		topic=topic.substr(pos+1);
		_snprintf(buf, sizeof(buf)-1, formatstr, s.c_str(), "");
		strm << buf;
	}
}

std::string ParamContainer::paramhelp(std::string param) const
{
	if(plist.find(param)->second.allowedtypes=="") return "<"+param+">";
	string s, t=plist.find(param)->second.allowedtypes;
	int pos=0, npos;
	while(1)
	{
		npos=(int)t.find_first_of("|", pos);
		if(npos==-1) break;
		if(tlist->find(t.substr(pos, npos-pos))->second.plist.size())
			s=s+t.substr(pos, npos-pos)+"[...]|";
		else
			s=s+t.substr(pos, npos-pos)+"|";
		pos=npos+1;
	}
	s=s+t.substr(pos);
	if(tlist->find(t.substr(pos))->second.plist.size())
		s=s+"[...]";
	return s;
}

std::string ParamContainer::typelist(std::string param) const
{
	string help=plist.find(param)->second.help;
	int tpos=(int)help.find("@types@");
	if(tpos==-1) return help;
	string s, t=plist.find(param)->second.allowedtypes;
	int pos=0, npos;
	while(1)
	{
		npos=(int)t.find_first_of("|", pos);
		if(npos==-1) break;
		s=s+t.substr(pos, npos-pos)+", ";
		pos=npos+1;
	}
	s=s+t.substr(pos);
	return help.substr(0, tpos)+s+help.substr(tpos+7);
}

/*
蔓忸?耧疣怅??镟疣戾蝠圊 ?蝈犟蝾恹?镱蝾?
*/
void ParamContainer::dumpHelp(FILE *f, bool showparamlist, unsigned int width, std::string subtopic) const
{
	// 罔痂磬 脲忸?觐腩黻?镳?恹忸溴 耧疣怅?
	unsigned int helpindent=0;
	unsigned int l;
	// 闰屦囹铕 镱 耧桉牦 镟疣戾蝠钼
	int i;
	char s[maxpnamelength*2+10];
	unsigned int pos;
	// 蔓忸?耧桉赅 镟疣戾蝠钼
	// 悟耱箫 磬 8 镱玷鲨?潆 牮囫铗?
	if(showparamlist)
	{
		printHelpTopic(f, helptopic.c_str(), 0, width, false);
		//		fprintf(f, "%s", helptopic.c_str());
		pos = (int)(helptopic.length()-helptopic.find_last_of("\n"));
		if(subtopic!="" && plist.size())
		{
			fprintf(f, "\n        %s[", subtopic.c_str());
			pos=9+(int)subtopic.length();
		}
	}
	if(!plist.size())
	{
		showparamlist=false;
		fprintf(f, "\n");
	}
	// 扬屦忄 - 徨琨?眄 镟疣戾蝠?
	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & noname)
		{
			sprintf(s, "%s ", paramhelp(vlist[i]).c_str());
			if(showparamlist)
			{
				pos += (int)strlen(s);
				if(width && (pos>width) )
				{
					pos = 8+(int)strlen(s);
					fprintf(f, "\n%8s", "");
				}
				fprintf(f, "%s", s);
			}
			l = (int)vlist[i].length()+7;
			if(l>helpindent) helpindent=l;
		}
	}
	// 项蝾?- 钺玎蝈朦睇?镟疣戾蝠?
	for(i=0; i<(int)vlist.size(); i++)
	{
		if((plist.find(vlist[i])->second.pflag & required) && !(plist.find(vlist[i])->second.pflag & noname))
		{
			sprintf(s, "-%c %s ", plist.find(vlist[i])->second.abbr, paramhelp(vlist[i]).c_str());
			if(showparamlist)
			{
				pos += (int)strlen(s);
				if(width && (pos>width) )
				{
					pos = 8+(int)strlen(s);
					fprintf(f, "\n%8s", "");
				}
				fprintf(f, "%s", s);
			}
			l = (int)vlist[i].length()*2+7;
			if(l>helpindent) helpindent=l;
		}
	}
	// 青蝈?怦?铖蜞朦睇?
	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & (noname|required)) continue;
		if(plist.find(vlist[i])->second.pflag & novalue)
		{
			sprintf(s, "-%c ", plist.find(vlist[i])->second.abbr);
			l=(int)vlist[i].length();
		} else
		{
			sprintf(s, "-%c %s ", plist.find(vlist[i])->second.abbr, paramhelp(vlist[i]).c_str());
			l=(int)vlist[i].length()*2+7;
		}
		if(showparamlist)
		{
			pos += (int)strlen(s);
			if(width && (pos>width) )
			{
				pos = 8+(int)strlen(s);
				fprintf(f, "\n%8s", "");
			}
			fprintf(f, "%s", s);
		}
		if(l>helpindent) helpindent = l;
	}
	if(showparamlist)
	{
		if(subtopic!="") fprintf(f, "\b]\n");
		else fprintf(f, "\n\n");
	}
	// 吾?耥屙桢 赅驿钽?镟疣戾蝠?
	char formatstr[10];
	sprintf(formatstr, "%%-%ds", helpindent);
	if(width<(helpindent+20)) width = helpindent+20;
	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & noname)
		{
			fprintf(f, " %c     ", plist.find(vlist[i])->second.pflag & required?'*':' ');
			sprintf(s, "<%s> ", vlist[i].c_str(), vlist[i].c_str());
			fprintf(f, formatstr, s);
			printHelpTopic(f, typelist(vlist[i]), helpindent+7, width);
		}
	}
	for(i=0; i<(int)vlist.size(); i++)
	{
		if((plist.find(vlist[i])->second.pflag & required) && !(plist.find(vlist[i])->second.pflag & noname))
		{
			if(plist.find(vlist[i])->second.abbr)
				fprintf(f, " * -%c, ", plist.find(vlist[i])->second.abbr);
			else
				fprintf(f, " *     ");
			sprintf(s, "--%s=<%s> ", vlist[i].c_str(), vlist[i].c_str());
			fprintf(f, formatstr, s);
			printHelpTopic(f, typelist(vlist[i]), helpindent+7, width);
		}
	}
	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & (noname|required)) continue;
		if(plist.find(vlist[i])->second.abbr)
			fprintf(f, "   -%c, ", plist.find(vlist[i])->second.abbr);
		else
			fprintf(f, "       ");
		if(plist.find(vlist[i])->second.pflag & novalue)
			sprintf(s, "--%s ", vlist[i].c_str());
		else
			sprintf(s, "--%s=<%s> ", vlist[i].c_str(), vlist[i].c_str());
		fprintf(f, formatstr, s);
		printHelpTopic(f, typelist(vlist[i]), helpindent+7, width);
	}
	fprintf(f, "\n");
	if(dumpsubparamhelp)
	{
		for(i=0; i<(int)tindex.size(); i++)
			tlist->find(tindex[i])->second.dumpHelp(f, true, width, tlist->find(tindex[i])->first);
	}
	if(subtopic=="")
	{
		fprintf(f, " * = %s\n", messages[msgRequiredParameter]);
	}
}

void ParamContainer::dumpHelp( std::ostream& strm, bool showparamlist/*=true*/, unsigned int width/*=0*/, std::string subtopic/*=""*/ ) const
{
	unsigned int helpindent=0;
	unsigned int l;
	int i;
	char s[maxpnamelength*2+10];
	unsigned int pos;

	char buf[512] = {0};

	if(showparamlist)
	{
		printHelpTopic(strm, helptopic.c_str(), 0, width, false);
		pos = (int)(helptopic.length()-helptopic.find_last_of("\n"));
		if(subtopic!="" && plist.size())
		{
			_snprintf(buf, sizeof(buf)-1, "\n        %s[", subtopic.c_str());
			strm << buf;
			pos=9+(int)subtopic.length();
		}
	}
	if(!plist.size())
	{
		showparamlist=false;
		strm << endl;
	}

	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & noname)
		{
			sprintf(s, "%s ", paramhelp(vlist[i]).c_str());
			if(showparamlist)
			{
				pos += (int)strlen(s);
				if(width && (pos>width) )
				{
					pos = 8+(int)strlen(s);

					char buf[512] = {0};
					_snprintf(buf, sizeof(buf)-1, "\n%8s", "");
					strm << buf;
				}
				strm << s;
			}
			l = (int)vlist[i].length()+7;
			if(l>helpindent) helpindent=l;
		}
	}
	// 项蝾?- 钺玎蝈朦睇?镟疣戾蝠?
	for(i=0; i<(int)vlist.size(); i++)
	{
		if((plist.find(vlist[i])->second.pflag & required) && !(plist.find(vlist[i])->second.pflag & noname))
		{
			sprintf(s, "-%c %s ", plist.find(vlist[i])->second.abbr, paramhelp(vlist[i]).c_str());
			if(showparamlist)
			{
				pos += (int)strlen(s);
				if(width && (pos>width) )
				{
					pos = 8+(int)strlen(s);
					_snprintf(buf, sizeof(buf)-1, "\n%8s", "");
					strm << buf;
				}
				strm << s;
			}
			l = (int)vlist[i].length()*2+7;
			if(l>helpindent) helpindent=l;
		}
	}
	// 青蝈?怦?铖蜞朦睇?
	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & (noname|required)) continue;
		if(plist.find(vlist[i])->second.pflag & novalue)
		{
			sprintf(s, "-%c ", plist.find(vlist[i])->second.abbr);
			l=(int)vlist[i].length();
		} else
		{
			sprintf(s, "-%c %s ", plist.find(vlist[i])->second.abbr, paramhelp(vlist[i]).c_str());
			l=(int)vlist[i].length()*2+7;
		}
		if(showparamlist)
		{
			pos += (int)strlen(s);
			if(width && (pos>width) )
			{
				pos = 8+(int)strlen(s);

				_snprintf(buf, sizeof(buf)-1, "\n%8s", "");
				strm << buf;
			}
			strm << s;
		}
		if(l>helpindent) helpindent = l;
	}
	if(showparamlist)
	{
		if(subtopic!="")
			strm << "\b]\n";
		else 
			strm << "\n\n";
	}
	// 吾?耥屙桢 赅驿钽?镟疣戾蝠?
	char formatstr[10];
	sprintf(formatstr, "%%-%ds", helpindent);
	if(width<(helpindent+20)) width = helpindent+20;
	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & noname)
		{
			_snprintf(buf, sizeof(buf)-1, " %c     ", plist.find(vlist[i])->second.pflag & required?'*':' ');
			strm << buf;

			sprintf(s, "<%s> ", vlist[i].c_str(), vlist[i].c_str());
			_snprintf(buf, sizeof(buf)-1, formatstr, s);
			strm << buf;

			printHelpTopic(strm, typelist(vlist[i]), helpindent+7, width);
		}
	}
	for(i=0; i<(int)vlist.size(); i++)
	{
		if((plist.find(vlist[i])->second.pflag & required) && !(plist.find(vlist[i])->second.pflag & noname))
		{
			if(plist.find(vlist[i])->second.abbr)
			{
				_snprintf(buf, sizeof(buf)-1, " * -%c, ", plist.find(vlist[i])->second.abbr);
				strm << buf;
			}
			else
			{
				strm << " *     ";
			}
			sprintf(s, "--%s=<%s> ", vlist[i].c_str(), vlist[i].c_str());
			_snprintf(buf, sizeof(buf)-1, formatstr, s);
			strm << buf;

			printHelpTopic(strm, typelist(vlist[i]), helpindent+7, width);
		}
	}
	for(i=0; i<(int)vlist.size(); i++)
	{
		if(plist.find(vlist[i])->second.pflag & (noname|required)) continue;
		if(plist.find(vlist[i])->second.abbr)
		{
			_snprintf(buf, sizeof(buf)-1, "   -%c, ", plist.find(vlist[i])->second.abbr);
			strm << buf;
		}
		else
			strm << "       ";
		if(plist.find(vlist[i])->second.pflag & novalue)
			sprintf(s, "--%s ", vlist[i].c_str());
		else
			sprintf(s, "--%s=<%s> ", vlist[i].c_str(), vlist[i].c_str());
		_snprintf(buf, sizeof(buf)-1, formatstr, s);
		strm << buf;
		
		printHelpTopic(strm, typelist(vlist[i]), helpindent+7, width);
	}
	strm << endl;
	if(dumpsubparamhelp)
	{
		for(i=0; i<(int)tindex.size(); i++)
			tlist->find(tindex[i])->second.dumpHelp(strm, true, width, tlist->find(tindex[i])->first);
	}
	if(subtopic=="")
	{
		_snprintf(buf, sizeof(buf)-1, " * = %s\n", messages[msgRequiredParameter]);
		strm << buf;
	}
}
/*
蔓忸?耧桉赅 镟疣戾蝠钼 ?镳桉忸屙睇扈 桁 珥圜屙?扈
*/
void ParamContainer::saveParams(FILE *f, std::string signature) const
{
	if(signature!="") fprintf(f, "[%s]\n", signature.c_str());
	// 闰屦囹铕 镱 耧桉牦 镟疣戾蝠钼
	map<string, param>::const_iterator it;
	for(it=plist.begin(); it!=plist.end(); it++)
	{
		if(it->second.pflag & novalue)
		{
			if(it->second.wasset) fprintf(f, "%s=%s\n", it->first.c_str(), it->second.value.c_str());
		}
		else
		{
			if(it->second.p)
				fprintf(f, "%s=%s[%s]\n", it->first.c_str(), it->second.value.c_str(), it->second.p->getCmdLine().c_str());
			else
				if(it->second.wasset) 
					fprintf(f, "%s=\"%s\"\n", it->first.c_str(), it->second.value.c_str());
		}
	}
}

/*
杨躔囗屙桢 羿殡?镳铄牝?
*/
ParamContainer::errcode ParamContainer::saveParams(std::string filename, std::string signature)
{
	lastcmdline="";
	errpos=0;
	errinfo=filename;
	FILE *f=fopen(filename.c_str(), "wt");
	if(!f) return errUnableToOpenFile;
	char cwd[1024];
	getcwd(cwd, 1024);
	strcat(cwd, cwd[0]=='/'?"/":"\\");
	relToAbsPath(cwd, filename);
	convertFilePaths(cwd, filename);
	saveParams(f, signature);
	fclose(f);
	return errOk;
}

/*
青沭箸栩?镟疣戾蝠?桤 羿殡?镳铄牝??镳孱狃噻钼囗桢?桁疙 羿殡钼 ?噌耦膻蝽
*/
ParamContainer::errcode ParamContainer::loadParams(std::string filename, std::string signature)
{
	lastcmdline="";
	errpos=0;
	errinfo=filename;
	FILE *f=fopen(filename.c_str(), "rt");
	if(!f) return errUnableToOpenFile;
	errcode err=loadParams(f, signature);
	fclose(f);
	if(err!=errOk) return err;
	char cwd[1024];
	getcwd(cwd, 1024);
	strcat(cwd, cwd[0]=='/'?"/":"\\");
	relToAbsPath(cwd, filename);
	convertFilePaths(filename, "");
	return errOk;
}

/*
青沭箸栩?镟疣戾蝠?桤 羿殡?镳铄牝?
*/
ParamContainer::errcode ParamContainer::loadParams(FILE *f, std::string signature)
{
	int i=0;
	char str[1024];
	string s;
	if(signature!="")
	{
		fgets(str, 1024, f);
		if(!*str) return errInvalidSignature;
		if(str[strlen(str)-1]=='\n') str[strlen(str)-1]=0;
		s=str;
		if(s!="["+signature+"]") return errInvalidSignature;
	}
	// 厌痤?耧桉赅 镟疣戾蝠钼
	reset();
	s="";
	while(!feof(f))
	{
		// 阻蜞屐 镱 耱痤麝?桤 羿殡?
		i++;
		*str=0;
		fgets(str, 1024, f);
		if(str[strlen(str)-1]=='\n') str[strlen(str)-1]=0;
		if(!str[0]) continue;
		s=s+"--"+str+" ";
		if(s.length()>1048576)
			return errInvalidSyntax;
	}
	lastcmdline=s;
	errcode err=lexicalAnalysis(s);
	if(err!=errOk) return err;
	return syntaxAndSemanticsAnalysis(true);
}

void ParamContainer::convertFilePaths(std::string dirfrom, std::string dirto)
{
	// 闰屦囹铕 镱 耧桉牦 镟疣戾蝠钼
	map<string, param>::iterator it;
	for(it=plist.begin(); it!=plist.end(); it++)
	{
		if((it->second.pflag & filename) && it->second.value!="")
		{
			// 相疣戾蝠 - 礤矬耱铄 桁 羿殡?- 镳孱狃噻箦?
			relToAbsPath(dirfrom, it->second.value);
			absToRelPath(dirto, it->second.value);
		}
		if(it->second.p)
		{
			// 橡钿咫囹?蝾 驽 襦祛?潆 怆铈屙睇?ParamContainer
			it->second.p->convertFilePaths(dirfrom, dirto);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
#include <iostream>
using namespace std;
int test_getopt(int argc, char* argv[])
{
	ParamContainer p;

	// programname --long-name=value
	// programname -n value
	// programname -n "value"
// 	p.addParam("long-name", 'n', ParamContainer::regular, 
// 		"parameter description", "default_value");  

	//--sln=BugLint.sln --lnt=BugLint.lnt --listfile
	p.addParam("sln", 0, ParamContainer::filename, ".sln路径");  
	p.addParam("vcproj", 0, ParamContainer::filename, "vcproj.路径");  
	p.addParam("lnt", 0, ParamContainer::filename, ".lnt路径");  
	p.addParam("listfile", 0, ParamContainer::novalue, "将文件列表生成到lnt");  
	p.parseCommandLine(argc, argv);

	cout << p["sln"] << endl;
	cout << p["vcproj"] << endl;
	cout << p["lnt"] << endl;
	cout << p["listfile"] << endl;
	cout << p.is_set("listfile") << endl;
	return 0;
}
//////////////////////////////////////////////////////////////////////////
