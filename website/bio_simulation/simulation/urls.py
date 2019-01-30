from django.conf.urls import url

from . import views
from django.contrib import admin

urlpatterns = [
    url(r'^admin/', admin.site.urls),
    
    url(r'^login/$', views.login, name='login'),
    url(r'^logout/$', views.logout, name='logout'),
    
    url(r'^$', views.index, name='index'),
    url(r'^doc/$', views.doc, name='doc'),
    
    url(r'^createuser/$', views.createuser, name='createuser'),
    
    url(r'^models/$', views.models, name='models'),
    url(r'^models/load_model/$', views.load_model),
    url(r'^models/load_simulation/$', views.load_simulation),
    url(r'^models/save/$', views.save, name='save'),
    url(r'^models/copy/$', views.copy, name='copy'),
    url(r'^models/delete/$', views.delete, name='delete'),
    url(r'^models/download_output/$', views.download_output, name='download_output'),
]
