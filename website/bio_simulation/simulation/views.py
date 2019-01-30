from django.shortcuts import render, redirect
from django.http import JsonResponse
from django.contrib.auth import authenticate, login as auth_login, logout as auth_logout
from django.contrib.auth.models import User
from django.conf import settings as django_settings

import os
import zipfile
import StringIO
import shutil

import time
import json
from .models import Model, Simulations

from django.http import HttpResponse
import csv

# For the moment, need to put simulation_client.pyc in the folder... not good, should install the module
from simulation_client import SimulationClient
from simulation_api import SimulationApi

import threading

def index(request):
    """This is the index page, with :
    - General description
    - Account creation"""
    # A context is used when the account can not be created
    context = {}
    if request.POST.get("user_exists"):
        context["user_exists"] = True
    elif request.POST.get("password_length"):
        context["password_length"] = True
    if request.user:
        context["username"] = request.user.username
    if request.POST.get("unsuccessful_auth"):
        context["unsuccessful_auth"] = True
    return render(request, 'simulation/index.html', context)

def doc(request):
    """The documentation page.
    """
    context = {}
    if request.user:
        context["username"] = request.user.username
    return render(request, 'simulation/doc.html', context)

def createuser(request):
    """Create a user in the database.
    """
    # Retrieve username and password
    username = request.POST.get("create_username", None)
    password = request.POST.get("create_password", None)
    # TODO : find a better way to send error message in index page
    # By default, request.POST object is immutable, very ugly to make it mutable like this, but it works. To improve.
    request.POST._mutable = True
    if User.objects.filter(username=username).exists():
        # The user exists : reload index page with corresponding error message
        request.POST["user_exists"] = True
        # Here we should use redirect(index) to be sure not having CSRF problems. But POST parameters are not passed this way...
        return index(request)
    elif len(password)==0:
        # The password length is bad : reload index page with corresponding error message
        request.POST["password_length"] = True
        # Here we should use redirect(index) to be sure not having CSRF problems. But POST parameters are not passed this way...
        return index(request)
    else:
        # Create the user in the database and login
        user = User.objects.create_user(username, '', password)
        user.save()
        auth_login(request, user)
        # Add the models for this user
        for model_id in django_settings.MODELS_ID_AT_CREATION:
            # Get model and settings in database
            models_to_add = Model.objects.filter(id=model_id)
            if len(models_to_add)>0:
                model_to_add = models_to_add[0]
                model, settings = model_to_add.model, model_to_add.settings
                # Insert them in the database for this user
                model_added = Model.objects.create(model=model, settings=settings, user_id=request.user.id)
                model_added.save()
                model_added_id = model_added.id
                # Run the simulation for this model and push the outputs
                threading.Thread(target=run_and_push, args=(model_added_id, json.loads(model), json.loads(settings)), kwargs={}).start()
        return redirect(models)

def login(request):
    """Login
    """
    # Authentication
    username = request.POST.get("username", None)
    password = request.POST.get("password", None)
    user = authenticate(username=username, password=password)
    if user:
        # Successful authentication
        auth_login(request, user)
        return redirect(models)
    else:
        # Unsuccessful authentication
        request.POST._mutable = True
        request.POST["unsuccessful_auth"] = True
        return index(request)

def logout(request):
    """Logout
    """
    auth_logout(request)
    return redirect(index)

def models(request):
    """Retrieve the models list in the db to be able to display them.
    """
    models = Model.objects.filter(user_id=request.user.id).order_by("id")
    context = { "models_list": [{"id": m.id, "model": json.loads(m.model)} for m in models] }
    if request.user:
        context["username"] = request.user.username
    # Set the active model in the context, determined the following way :
    # - the value in the session (if the model has just been created)
    # -  the first in the list
    # - None if no model exists
    created_model_id = request.session.get("created_model_id", None)
    if created_model_id!=None:
        context["active_model_id"] = created_model_id
        request.session["created_model_id"] = None
    else:
        if len(models)>0:
            context["active_model_id"] = models[0].id
        else:
            context["active_model_id"] = None
    return render(request, 'simulation/models.html', context)

def load_model(request):
    """Load the model"""
    model_id = int(request.POST.get("model_id", None))
    result = {}
    model = Model.objects.get(pk=model_id)
    result["model"] = json.loads(model.model)
    result["settings"] = json.loads(model.settings)
    return JsonResponse(result)

def load_simulation(request):
    """Load the simulation (and also the model (and settings) because we need (we might need) it in the simulation plot)."""
    model_id = int(request.POST.get("model_id", None))
    result = {}
    model = Model.objects.get(pk=model_id)
    result["model"] = json.loads(model.model)
    result["settings"] = json.loads(model.settings)
    try:
        simulations = Simulations.objects.filter(model_id=model_id).order_by("-id")
        if len(simulations)==0:
            # The simulation did not finish, wait a few more seconds
            time.sleep(5)
            simulations = Simulations.objects.filter(model_id=model_id).order_by("-id")
        simulation = simulations[0]
        result["simulation"] = json.loads(simulation.simulation)
        result["lineages"] = SimulationApi.get_lineages(result["simulation"]["output"])
    except (IndexError, KeyError):
        # IndexError happens when there are no simulations
        # KeyError happens when the simulation has failed
        pass
    return JsonResponse(result)

def save(request):
    """ Saves the model in the database and runs the simulation.
    - If an id is given in the POST request, it updates the model corresponding to this id
    - Otherwise, it creates a new model in the database
    """
    # Update / create the model in the database
    model_json = request.POST.get("model", None)
    settings_json = request.POST.get("settings", None)
    model_id = request.POST.get("model_id", None)
    model = None
    if model_id!="":
        # The model exists already, we juste update it
        model = Model.objects.get(pk=model_id)
        model.model = model_json
        model.settings = settings_json
    else:
        # Create the model
        model = Model.objects.create(model=model_json, settings=settings_json, user_id=request.user.id)
    model.save()
    # Delete the existing simulations for this model
    Simulations.objects.filter(model_id=model.id).delete();
    # Run the simulation asynchronously for this model
    threading.Thread(target=run_and_push, args=(model.id, json.loads(model.model), json.loads(model.settings)), kwargs={}).start()
    # Set the model id in the session to be able to select it when reloading the models page
    request.session["created_model_id"] = model.id
    return redirect(models)

def copy(request):
    """ Copy the model in the database and runs the simulation.
    The model id is given in the POST request.
    """
    model_id = request.POST.get("model_id", None)
    if model_id:
        # Get model and settings in database
        model = Model.objects.filter(id=model_id)
        if len(model)>0:
            model_to_add = model[0]
            model, settings = model_to_add.model, model_to_add.settings
            # Change the name of the model
            model_changed = json.loads(model)
            model_changed['name'] = '{n} (copy)'.format(n=model_changed['name'])
            # Insert them in the database for this user
            model_added = Model.objects.create(model=json.dumps(model_changed), settings=settings, user_id=request.user.id)
            model_added.save()
            model_added_id = model_added.id
            # Set the model id in the session to be able to select it when reloading the models page
            request.session["created_model_id"] = model_added_id
            # Run the simulation for this model and push the outputs
            threading.Thread(target=run_and_push, args=(model_added_id, json.loads(model), json.loads(settings)), kwargs={}).start()
    return redirect(models)

def run_and_push(model_id, model, settings):
    """Run the simulation and push it in database."""
    client = SimulationClient(django_settings.SIMULATION_SERVER)
    output = client.simulate(model, settings)
    Simulations.objects.create(model_id=model_id, simulation=json.dumps(output))

def delete(request):
    """ Delete a model and the corresponding simulations from the database.
    """
    model_id = request.POST.get("model_id", None)
    if model_id!="":
        id_ = int(model_id)
        Model.objects.get(pk=id_).delete();
        Simulations.objects.filter(model_id=id_).delete();
    # Unset the active model id in the context
    request.session["created_model_id"] = None
    return redirect(models)

def download_output(request):
    """Download the output of the simulation as a zip with one csv file per lineage.
    - All csv files are created in a directory
    - The directory is zipped and returned
    - The unzipped directory is deleted
    """
    simulation_output = request.POST.get("simulation_output", None)
    model_name = request.POST.get("model_name_in_form", None)
    model_molecules = request.POST.get("model_molecules_in_form", None)
    # Create the zip archive
    dir_name = '{n}_{t}_output'.format(n=json.loads(model_name), t=int(time.time()))
    root_name = '/var/tmp/{d}'.format(d=dir_name)
    os.mkdir(root_name)
    out = StringIO.StringIO()
    zf = zipfile.ZipFile(out, "w")
    zip_name = '{d}.zip'.format(d=dir_name)

    if simulation_output and model_name and model_molecules:
        # Parse the output
        output = json.loads(simulation_output)
        molecules = json.loads(model_molecules)
        # Create the csv files (one per lineage)
        header = ['t', 'V'] + ['q_{n}'.format(n=i['name']) for i in molecules]
        for e,l in enumerate(output):
            fname = 'lineage_{e}.csv'.format(e=str(e))
            with open('{d}/{f}'.format(d=root_name, f=fname), 'w') as f:
                writer = csv.writer(f)
                writer.writerow(header)
                for i in l:
                    writer.writerow(i)
        # Add the files in the zip archive
        for fname in os.listdir(root_name):
            zf.write('{d}/{n}'.format(d=root_name, n=fname), arcname=fname)

    # Return the zip
    shutil.rmtree(root_name)
    zf.close()
    response = HttpResponse(out.getvalue(), content_type = "application/x-zip-compressed")
    response['Content-Disposition'] = 'attachment; filename="{n}"'.format(n=zip_name)
    return response
