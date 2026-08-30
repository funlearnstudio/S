#include "s/game_ext.hpp"
#include "s/error.hpp"
#include "s/interpreter.hpp"

#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace s {
namespace {

std::shared_ptr<CallableData> gamex_callable(std::string name,std::size_t min,std::size_t max,
    std::function<Value(const std::vector<Value>&,SourcePos)> fn,bool variadic=false){
  auto c=std::make_shared<CallableData>();
  c->name=std::move(name);c->min_args=min;c->max_args=max;c->variadic=variadic;c->call=std::move(fn);return c;
}
std::shared_ptr<FunctionSig> gamex_sig(std::vector<TypeInfo> params,TypeInfo result,bool variadic=false,std::size_t min=0,bool fallible=false){
  auto s=std::make_shared<FunctionSig>();s->params=std::move(params);s->result=std::move(result);s->variadic=variadic;s->min_args=min;s->fallible=fallible;return s;
}
TypeInfo gamex_fn(std::vector<TypeInfo> params,TypeInfo result,bool variadic=false,std::size_t min=0,bool fallible=false){
  TypeInfo t(TypeKind::Function);t.callable=gamex_sig(std::move(params),std::move(result),variadic,min,fallible);return t;
}

double gamex_num(const Value& v,SourcePos p,const std::string& name){
  if(auto x=std::get_if<std::int64_t>(&v.data()))return static_cast<double>(*x);
  if(auto x=std::get_if<double>(&v.data()))return *x;
  throw Error(p,name+" needs a number.");
}
std::int64_t gamex_int(const Value& v,SourcePos p,const std::string& name){
  if(auto x=std::get_if<std::int64_t>(&v.data()))return *x;
  throw Error(p,name+" needs Int.");
}
std::string gamex_text(const Value& v,SourcePos p,const std::string& name){
  if(auto x=std::get_if<std::string>(&v.data()))return *x;
  throw Error(p,name+" needs Text.");
}
bool gamex_bool(const Value& v,SourcePos p,const std::string& name){
  if(auto x=std::get_if<bool>(&v.data()))return *x;
  throw Error(p,name+" needs Bool.");
}
std::string gamex_js(const std::string& text){
  std::string out;
  for(char c:text){
    if(c=='\\')out+="\\\\";
    else if(c=='\"')out+="\\\"";
    else if(c=='\n')out+="\\n";
    else if(c=='\r')out+="\\r";
    else out+=c;
  }
  return out;
}
void gamex_script(const std::shared_ptr<ModuleData>& module,std::int64_t scene,const std::string& script,SourcePos p){
  auto it=module->exports.find("script");
  if(it==module->exports.end())throw Error(p,"game runtime has no script bridge.");
  auto fn=std::get_if<std::shared_ptr<CallableData>>(&it->second.data());
  if(!fn||!*fn)throw Error(p,"game script bridge is unavailable.");
  (*fn)->call({Value(scene),Value(script)},p);
}

std::string gamex_sprite_init(){
  return "SEGame.sprites??=new Map();SEGame.sounds??=new Map();SEGame.particles??=[];SEGame.camera??={x:0,y:0};";
}

} // namespace

void extend_game_type(TypeInfo& module){
  auto& x=module.members;TypeInfo none(TypeKind::None),num(TypeKind::Num),integer(TypeKind::Int),text(TypeKind::Text),boolean(TypeKind::Bool),list(TypeKind::List);list.element=std::make_shared<TypeInfo>(num);
  x["image"]=gamex_fn({integer,text,num,num,num,num},none);
  x["sprite"]=gamex_fn({integer,text,text,num,num,num,num},none);
  x["sprite_color"]=gamex_fn({integer,text,num,num,num,num,text},none);
  x["position"]=gamex_fn({integer,text,num,num},none);x["move"]=gamex_fn({integer,text,num,num},none);x["velocity"]=gamex_fn({integer,text,num,num},none);
  x["animate"]=gamex_fn({integer,num},none);x["key_move"]=gamex_fn({integer,text,text,num,num},none);x["follow_mouse"]=gamex_fn({integer,text},none);
  x["sound"]=gamex_fn({integer,text,text},none);x["play"]=gamex_fn({integer,text,boolean,num},none);x["stop"]=gamex_fn({integer,text},none);
  x["fullscreen"]=gamex_fn({integer},none);x["camera"]=gamex_fn({integer,num,num},none);
  x["particles"]=gamex_fn({integer,num,num,integer,text,num},none);
  x["rect_hit"]=gamex_fn({num,num,num,num,num,num,num,num},boolean);x["circle_hit"]=gamex_fn({num,num,num,num,num,num},boolean);x["distance"]=gamex_fn({num,num,num,num},num);x["vector"]=gamex_fn({num,num},list);
}

void extend_game_module(const std::shared_ptr<ModuleData>& module,Interpreter& vm){
  (void)vm;
  module->exports["image"]=gamex_callable("game.image",6,6,[module](const std::vector<Value>& a,SourcePos p){
    auto scene=gamex_int(a[0],p,"game.image");std::ostringstream js;js<<"{const i=new Image();i.src=\""<<gamex_js(gamex_text(a[1],p,"game.image"))<<"\";i.onload=()=>ctx.drawImage(i,"<<gamex_num(a[2],p,"game.image")<<","<<gamex_num(a[3],p,"game.image")<<","<<gamex_num(a[4],p,"game.image")<<","<<gamex_num(a[5],p,"game.image")<<");}";gamex_script(module,scene,js.str(),p);return Value{};
  });
  module->exports["sprite"]=gamex_callable("game.sprite",7,7,[module](const std::vector<Value>& a,SourcePos p){
    auto scene=gamex_int(a[0],p,"game.sprite");auto name=gamex_js(gamex_text(a[1],p,"game.sprite"));auto url=gamex_js(gamex_text(a[2],p,"game.sprite"));std::ostringstream js;js<<gamex_sprite_init()<<"{const i=new Image();i.src=\""<<url<<"\";SEGame.sprites.set(\""<<name<<"\",{name:\""<<name<<"\",image:i,x:"<<gamex_num(a[3],p,"game.sprite")<<",y:"<<gamex_num(a[4],p,"game.sprite")<<",w:"<<gamex_num(a[5],p,"game.sprite")<<",h:"<<gamex_num(a[6],p,"game.sprite")<<",vx:0,vy:0,visible:true});}";gamex_script(module,scene,js.str(),p);return Value{};
  });
  module->exports["sprite_color"]=gamex_callable("game.sprite_color",7,7,[module](const std::vector<Value>& a,SourcePos p){
    auto scene=gamex_int(a[0],p,"game.sprite_color");auto name=gamex_js(gamex_text(a[1],p,"game.sprite_color"));auto color=gamex_js(gamex_text(a[6],p,"game.sprite_color"));std::ostringstream js;js<<gamex_sprite_init()<<"SEGame.sprites.set(\""<<name<<"\",{name:\""<<name<<"\",color:\""<<color<<"\",x:"<<gamex_num(a[2],p,"game.sprite_color")<<",y:"<<gamex_num(a[3],p,"game.sprite_color")<<",w:"<<gamex_num(a[4],p,"game.sprite_color")<<",h:"<<gamex_num(a[5],p,"game.sprite_color")<<",vx:0,vy:0,visible:true});";gamex_script(module,scene,js.str(),p);return Value{};
  });
  auto sprite_xy=[module](const std::string& op,const std::vector<Value>& a,SourcePos p,bool delta){auto scene=gamex_int(a[0],p,"game."+op);auto name=gamex_js(gamex_text(a[1],p,"game."+op));std::ostringstream js;js<<gamex_sprite_init()<<"{const s=SEGame.sprites.get(\""<<name<<"\");if(s){s.x"<<(delta?"+=":"=")<<gamex_num(a[2],p,"game."+op)<<";s.y"<<(delta?"+=":"=")<<gamex_num(a[3],p,"game."+op)<<";}}";gamex_script(module,scene,js.str(),p);return Value{};};
  module->exports["position"]=gamex_callable("game.position",4,4,[sprite_xy](const std::vector<Value>& a,SourcePos p){return sprite_xy("position",a,p,false);});
  module->exports["move"]=gamex_callable("game.move",4,4,[sprite_xy](const std::vector<Value>& a,SourcePos p){return sprite_xy("move",a,p,true);});
  module->exports["velocity"]=gamex_callable("game.velocity",4,4,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.velocity");auto name=gamex_js(gamex_text(a[1],p,"game.velocity"));std::ostringstream js;js<<gamex_sprite_init()<<"{const s=SEGame.sprites.get(\""<<name<<"\");if(s){s.vx="<<gamex_num(a[2],p,"game.velocity")<<";s.vy="<<gamex_num(a[3],p,"game.velocity")<<";}}";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["animate"]=gamex_callable("game.animate",2,2,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.animate");auto fps=gamex_num(a[1],p,"game.animate");if(fps<=0||fps>240)throw Error(p,"game.animate fps must be between 0 and 240.");std::ostringstream js;js<<gamex_sprite_init()<<"SEGame.fps="<<fps<<";SEGame.last=performance.now();function SEFrame(now){const dt=Math.min((now-SEGame.last)/1000,0.1);if(now-SEGame.last>=1000/SEGame.fps){SEGame.last=now;ctx.save();ctx.fillStyle='"<<"#111"<<"';ctx.fillRect(0,0,canvas.width,canvas.height);ctx.translate(-SEGame.camera.x,-SEGame.camera.y);for(const s of SEGame.sprites.values()){if(!s.visible)continue;s.x+=s.vx*dt;s.y+=s.vy*dt;if(s.image&&s.image.complete)ctx.drawImage(s.image,s.x,s.y,s.w,s.h);else if(s.color){ctx.fillStyle=s.color;ctx.fillRect(s.x,s.y,s.w,s.h);}}for(const q of SEGame.particles){q.x+=q.vx*dt;q.y+=q.vy*dt;q.life-=dt;ctx.fillStyle=q.color;ctx.fillRect(q.x,q.y,3,3);}SEGame.particles=SEGame.particles.filter(q=>q.life>0);ctx.restore();}requestAnimationFrame(SEFrame)}requestAnimationFrame(SEFrame);";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["key_move"]=gamex_callable("game.key_move",5,5,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.key_move");auto name=gamex_js(gamex_text(a[1],p,"game.key_move"));auto key=gamex_js(gamex_text(a[2],p,"game.key_move"));std::ostringstream js;js<<gamex_sprite_init()<<"addEventListener('keydown',e=>{if(e.key===\""<<key<<"\"){const s=SEGame.sprites.get(\""<<name<<"\");if(s){s.x+="<<gamex_num(a[3],p,"game.key_move")<<";s.y+="<<gamex_num(a[4],p,"game.key_move")<<";}}});";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["follow_mouse"]=gamex_callable("game.follow_mouse",2,2,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.follow_mouse");auto name=gamex_js(gamex_text(a[1],p,"game.follow_mouse"));std::ostringstream js;js<<gamex_sprite_init()<<"canvas.addEventListener('mousemove',()=>{const s=SEGame.sprites.get(\""<<name<<"\");if(s){s.x=SEGame.mouse.x-s.w/2;s.y=SEGame.mouse.y-s.h/2;}});";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["sound"]=gamex_callable("game.sound",3,3,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.sound");auto name=gamex_js(gamex_text(a[1],p,"game.sound"));auto url=gamex_js(gamex_text(a[2],p,"game.sound"));std::ostringstream js;js<<gamex_sprite_init()<<"SEGame.sounds.set(\""<<name<<"\",new Audio(\""<<url<<"\"));";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["play"]=gamex_callable("game.play",4,4,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.play");auto name=gamex_js(gamex_text(a[1],p,"game.play"));auto loop=gamex_bool(a[2],p,"game.play");auto volume=gamex_num(a[3],p,"game.play");if(volume<0||volume>1)throw Error(p,"game.play volume must be 0..1.");std::ostringstream js;js<<gamex_sprite_init()<<"{const s=SEGame.sounds.get(\""<<name<<"\");if(s){s.loop="<<(loop?"true":"false")<<";s.volume="<<volume<<";const p=s.play();if(p)p.catch(()=>addEventListener('pointerdown',()=>s.play(),{once:true}));}}";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["stop"]=gamex_callable("game.stop",2,2,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.stop");auto name=gamex_js(gamex_text(a[1],p,"game.stop"));gamex_script(module,scene,gamex_sprite_init()+"{const s=SEGame.sounds.get(\""+name+"\");if(s){s.pause();s.currentTime=0;}}",p);return Value{};});
  module->exports["fullscreen"]=gamex_callable("game.fullscreen",1,1,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.fullscreen");gamex_script(module,scene,"canvas.addEventListener('dblclick',()=>{if(!document.fullscreenElement)canvas.requestFullscreen?.();else document.exitFullscreen?.();});",p);return Value{};});
  module->exports["camera"]=gamex_callable("game.camera",3,3,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.camera");std::ostringstream js;js<<gamex_sprite_init()<<"SEGame.camera.x="<<gamex_num(a[1],p,"game.camera")<<";SEGame.camera.y="<<gamex_num(a[2],p,"game.camera")<<";";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["particles"]=gamex_callable("game.particles",6,6,[module](const std::vector<Value>& a,SourcePos p){auto scene=gamex_int(a[0],p,"game.particles");auto count=gamex_int(a[3],p,"game.particles");if(count<0||count>10000)throw Error(p,"game.particles count must be 0..10000.");auto color=gamex_js(gamex_text(a[4],p,"game.particles"));auto speed=gamex_num(a[5],p,"game.particles");std::ostringstream js;js<<gamex_sprite_init()<<"for(let i=0;i<"<<count<<";i++){const a=Math.random()*Math.PI*2,r=Math.random()*"<<speed<<";SEGame.particles.push({x:"<<gamex_num(a[1],p,"game.particles")<<",y:"<<gamex_num(a[2],p,"game.particles")<<",vx:Math.cos(a)*r,vy:Math.sin(a)*r,life:1+Math.random(),color:\""<<color<<"\"});}";gamex_script(module,scene,js.str(),p);return Value{};});
  module->exports["rect_hit"]=gamex_callable("game.rect_hit",8,8,[](const std::vector<Value>& a,SourcePos p){double ax=gamex_num(a[0],p,"game.rect_hit"),ay=gamex_num(a[1],p,"game.rect_hit"),aw=gamex_num(a[2],p,"game.rect_hit"),ah=gamex_num(a[3],p,"game.rect_hit"),bx=gamex_num(a[4],p,"game.rect_hit"),by=gamex_num(a[5],p,"game.rect_hit"),bw=gamex_num(a[6],p,"game.rect_hit"),bh=gamex_num(a[7],p,"game.rect_hit");return Value(ax<bx+bw&&ax+aw>bx&&ay<by+bh&&ay+ah>by);});
  module->exports["circle_hit"]=gamex_callable("game.circle_hit",6,6,[](const std::vector<Value>& a,SourcePos p){auto dx=gamex_num(a[0],p,"game.circle_hit")-gamex_num(a[3],p,"game.circle_hit");auto dy=gamex_num(a[1],p,"game.circle_hit")-gamex_num(a[4],p,"game.circle_hit");auto r=gamex_num(a[2],p,"game.circle_hit")+gamex_num(a[5],p,"game.circle_hit");return Value(dx*dx+dy*dy<=r*r);});
  module->exports["distance"]=gamex_callable("game.distance",4,4,[](const std::vector<Value>& a,SourcePos p){return Value(std::hypot(gamex_num(a[2],p,"game.distance")-gamex_num(a[0],p,"game.distance"),gamex_num(a[3],p,"game.distance")-gamex_num(a[1],p,"game.distance")));});
  module->exports["vector"]=gamex_callable("game.vector",2,2,[](const std::vector<Value>& a,SourcePos p){auto out=std::make_shared<ListData>();out->items.emplace_back(gamex_num(a[0],p,"game.vector"));out->items.emplace_back(gamex_num(a[1],p,"game.vector"));return Value(out);});
}

} // namespace s
