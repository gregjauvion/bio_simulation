# -*- coding: utf-8 -*-
# Generated by Django 1.10.2 on 2017-01-02 22:41
from __future__ import unicode_literals

import datetime
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('simulation', '0007_auto_20170102_2145'),
    ]

    operations = [
        migrations.AlterField(
            model_name='model',
            name='insertion_date',
            field=models.DateTimeField(default=datetime.datetime.now),
        ),
        migrations.AlterField(
            model_name='simulations',
            name='insertion_date',
            field=models.DateTimeField(default=datetime.datetime.now),
        ),
    ]