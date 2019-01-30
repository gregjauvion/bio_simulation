from __future__ import unicode_literals

from django.db import models
from django.contrib.auth.models import User
from datetime import datetime

# The definition of a model
class Model(models.Model):
	model = models.TextField()
	# This field contains a json with the settings of the simulation. Not very good to put them in the table with the models.
	# It is simple for the moment, while we enable only one simulation per model
	settings = models.TextField()
	user = models.ForeignKey(User)
	insertion_date = models.DateTimeField(default=datetime.now)

# The simulations performed
class Simulations(models.Model):
	model = models.ForeignKey(Model)
	simulation = models.TextField()
	insertion_date = models.DateTimeField(default=datetime.now)
