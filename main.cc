#include<gtkmm.h>
#include<stdio.h>
#include<regex>
#include<pthread.h>
#include<iostream>
#include<gdkmm.h>
#include<stdlib.h>
#include<fstream>
// #include<gdk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

std::string get_filename(std::string file){
    std::size_t index= file.rfind("/");
    if(index==-1) return "";
    // std::cout <<file<<index<<std::endl;
    std::size_t endindex = file.rfind(".");
    printf("end inx %li\n",endindex);
    if(endindex==-1) 
        return file.substr(index+1);
    // std::cout <<file.substr(index+1,endindex-index-1)<<" end"<<std::endl;
    return file.substr(index+1,endindex-index-1);
}


class win:public Gtk::Window
{

public:
    win();
    ~win();

    Gtk::Button *browse_button;
    Gtk::Button *compress_button;
    Gtk::RadioButton *high_compression_button;
    Gtk::RadioButton *medium_compression_button;
    Gtk::RadioButton *low_compression_button;
    Glib::RefPtr<Gtk::Builder> builder;
    Gtk::Box *main_box;
    Gtk::Grid *status_box;
    Gtk::Image *pdf_image,*compress_image;
    // Gtk::ListStore *list_store;
    Gtk::ComboBox *combobox;
    Gtk::Entry *file_add_entry;
    Gtk::Entry *pdf_name_entry;

    class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    { add(m_col_name); }
    //The data to choose - this must be text.
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };



    GtkWidget *image;
    ModelColumns columns;
    Glib::RefPtr<Gtk::ListStore> list_store;
    // GdkPixbufAnimation * animation;

    void on_browse_button();
    void on_compress_button();
    void on_radio_toggle(Gtk::RadioButton *);
    void on_combo_changed(Gtk::Entry *);
    void on_file_add_entry_activate();

    int compression=1;
};


win::win():
file_add_entry(NULL),
pdf_name_entry(NULL)
{
    builder=Gtk::Builder::create_from_file("mainui2.glade");
    // main_window=window;
    builder->get_widget("main_box",main_box);
    builder->get_widget("status_box",status_box);
    builder->get_widget("browse_button",browse_button);
    builder->get_widget("compress_button",compress_button);
    builder->get_widget("high_compression_radio",high_compression_button);
    builder->get_widget("medium_compression_radio",medium_compression_button);
    builder->get_widget("low_compression_radio",low_compression_button);
    builder->get_widget("pdf_image",pdf_image);
    builder->get_widget("compress_image",compress_image);
    builder->get_widget("add_file_combo",combobox);
    builder->get_widget("pdf_name_entry",pdf_name_entry);
    file_add_entry=combobox->get_entry();
    file_add_entry->signal_activate().connect(sigc::mem_fun(this,&win::on_file_add_entry_activate));

    list_store=Gtk::ListStore::create(columns);
    // Gtk::TreeModel::Row row = *(list_store->append());
    combobox->set_model(list_store);
    
    combobox->signal_changed().connect(sigc::bind(sigc::mem_fun(this,&win::on_combo_changed),file_add_entry));

    browse_button->signal_clicked().connect(sigc::mem_fun(this,&win::on_browse_button));
    browse_button->set_margin_left(10);

    compress_button->signal_clicked().connect(sigc::mem_fun(this,&win::on_compress_button));
    compress_button->set_margin_right(10);

    // Glib::RefPtr<Gdk::Pixbuf> pixbuf=Gdk::Pixbuf::create_from_file("resources/pdff.png");
    // pixbuf->scale_simple(1,1,Gdk::INTERP_BILINEAR);
    // pdf_image->set(pixbuf);
    
    GdkPixbuf *pixel=gdk_pixbuf_new_from_file_at_scale("resources/pdff.png",30,30,true,NULL);
    gtk_image_set_from_pixbuf(pdf_image->gobj(),pixel);

    pixel=gdk_pixbuf_new_from_file_at_scale("resources/compress_pdf.png",35,50,true,NULL);
    gtk_image_set_from_pixbuf(compress_image->gobj(),pixel);
    
    // compress_image->set("resources/compress_pdf.svg");

    medium_compression_button->join_group(*high_compression_button);
    low_compression_button->join_group(*high_compression_button);

    low_compression_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(this,&win::on_radio_toggle),low_compression_button));
    medium_compression_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(this,&win::on_radio_toggle),medium_compression_button));
    high_compression_button->signal_clicked().connect(sigc::bind(sigc::mem_fun(this,&win::on_radio_toggle),high_compression_button));

    // file_label->set_max_width_chars(50);

    image=gtk_image_new();
    compress_button->set_sensitive(false);


    // gtk_image_set_from_file(GTK_IMAGE(image),"resources/inboxx.png");
    auto animation = gdk_pixbuf_animation_new_from_file ("resources/converting.gif", NULL);
    gtk_image_set_from_animation (GTK_IMAGE(image), GDK_PIXBUF_ANIMATION(animation));

    // option_box->pack_start(*gif,false,false,0);
    add(*main_box);
    printf("sdf");
    set_resizable(false);
    show_all();
    // gtk_box_pack_start(status_box->gobj(),image,false,false,0); 
    gtk_grid_attach(status_box->gobj(),image,0,0,1,1);
    
    // gtk_widget_hide(image);
}

win::~win(){
    std::cout<<"destructor\n";
}

void win::on_browse_button(){
    Gtk::FileChooserDialog fdialog("Choose Pdf File",Gtk::FILE_CHOOSER_ACTION_OPEN);
    fdialog.set_transient_for(*this);
    fdialog.add_button("_Cancel",Gtk::RESPONSE_CANCEL);
    fdialog.add_button("Open",Gtk::RESPONSE_OK);
    int result=fdialog.run();
    switch(result){
        case(Gtk::RESPONSE_OK):
        {
            std::string filename=fdialog.get_filename();
            file_add_entry->set_text(filename);
            compress_button->set_sensitive(true);
            pdf_name_entry->set_text(get_filename(filename)+"_compressed");
            break;
        }
        case(Gtk::RESPONSE_CANCEL):
        {
            printf("canceled\n");
            break;
        }
        default:
        {
            printf("Not selected\n");
            break;
        }
    }
    // g_object_unref(fdialog.gobj());
    printf("Browse\n");
}

std::string command;

void* compressor(void* c){
    win* ptr=(win*)c;
    std::cout<<command<<ptr->compression<<"\n";
    system(command.c_str());

    // gtk_image_clear(GTK_IMAGE(ptr->image));
    gtk_widget_hide(ptr->image);
    printf("done compression\n");
    return NULL;

}

void win::on_combo_changed(Gtk::Entry *entry){

    std::cout<<"on_combo_changed"<<entry->get_text()<<std::endl;
    on_file_add_entry_activate();
}


void win::on_file_add_entry_activate(){
    std::string s=file_add_entry->get_text();
    if(s=="") return;
    std::cout<<"entry activated "<<s<<std::endl;
    std::regex r("~/");
    s=std::regex_replace(s,r,std::string(getenv("HOME"))+"/");
    if(is_regular_file(s.c_str()))
    {   
        bool insert=true;
        for(auto row:list_store->children()){
            if(row[columns.m_col_name]==s){
                insert=false;
                break;
            }
        }
        if(insert){
        Gtk::TreeModel::Row row = *(list_store->append());
        row.set_value(0,s);
        }
        compress_button->set_sensitive(true);
        pdf_name_entry->set_text(get_filename(s)+"_compressed");
    }
    else{
        std::cout<<"file not found\n"<<std::endl;
        compress_button->set_sensitive(false);
    }
}

void win::on_compress_button(){
    pthread_t thread;

    printf("compressing\n");
    std::regex r(" ");
    std::string s=file_add_entry->get_text();
    s=std::regex_replace(s,r,"\\ ");
    auto s2=std::regex_replace(std::string(pdf_name_entry->get_text()),r,"\\ ");
    command=("./compress-button.sh "+ std::to_string(compression) + " "+s+" "+s2);

    

    
    // auto simple_anim=gdk_pixbuf_simple_anim_new(100,20,1);
    // auto going=true;
    // while(going){
    //     auto iter=gdk_pixbuf_animation_get_iter(animation,NULL);
    //     auto pixbuf=gdk_pixbuf_animation_iter_get_pixbuf(iter);
    //     auto newpix=gdk_pixbuf_scale_simple(pixbuf,100,20,GDK_INTERP_NEAREST);
    //     going=gdk_pixbuf_animation_iter_advance(iter,NULL);
    //     gdk_pixbuf_simple_anim_add_frame (simple_anim,newpix);
    //     std::cout<<"count"<<going<<"\n";
    // }
    // gdk_pixbuf_simple_anim_set_loop (simple_anim,true);

    gtk_widget_show(image);

    pthread_create(&thread,NULL,&compressor,(void*)this);

    // pthread_join(thread,NULL);
    // system(("./compress-button.sh "+ std::to_string(compression) + " "+s).c_str());
    // printf("done compression\n");
}

void win::on_radio_toggle(Gtk::RadioButton *button){
    if(button->get_active()){
        compression=std::stoi(button->get_name());
      
    }
    printf("compression selection %i\n",compression);
}


int main(){
    auto app=Gtk::Application:: create("com.compress.pdf");
    // Glib::RefPtr<Gtk::Builder> builder=Gtk::Builder::create_from_file("mainui.glade");
    // Gtk::Window *main_window;
    // builder->get_widget("main_window",main_window);
    win exp;
    app->run(exp);
    std::cout<<"ENDED\n";
    return 0;
}