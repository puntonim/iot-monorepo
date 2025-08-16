"""
 ** IoT BE **

The `app` var in this module is the entry point.

This project should be run with:
 - in DEV: make run-dev  # Or: fastapi dev iot_be/main.py
 - in PROD with Monit: sudo monit start gunicorn-iot-be

In DEV the BE is served with FastAPI directly.
In PROD the BE is served with Gunicorn + Uvicorn workers as recommended:
 https://www.uvicorn.org/#running-with-gunicorn
"""

from contextlib import asynccontextmanager

import datetime_utils
from fastapi import FastAPI

from .conf import settings
from .remote_log import remote_log_router
from .temp import temp_router


@asynccontextmanager
async def lifespan(app: FastAPI):
    """
    The code before the yield will run ONCE, BEFORE the app starts RECEIVING requests.

    The code after the yield will run ONCE, AFTER having handled the last request and
     before SHUTTING DOWN.

     Docs: https://fastapi.tiangolo.com/advanced/events/
    """
    # # Serve the FE pages with FastAPI in DEV only (not in PROD). That is because
    # #  CORS doesn't work If you open index.html with a browser directly from the file
    # #  system.
    # if settings.ENV == EnvEnum.DEV:
    #     # If this mount was done at module level, then it should have come after all
    #     #  other routes defined (so at the bottom of this file).
    #     app.mount(
    #         "/", StaticFiles(directory=settings.FE_WWW_DIR, html=True), name="static"
    #     )

    _create_db_csv_files()

    yield  # Accept requests.

    # Do stuff before the app shuts down.


app = FastAPI(lifespan=lifespan)
app.include_router(temp_router.router)
app.include_router(remote_log_router.router)


@app.get("/iot/health")
async def health_endpoint():
    now = datetime_utils.now_utc().isoformat()
    return now


def _create_db_csv_files():
    if not settings.CSV_DB_CLIMATE_PATH.exists():
        with open(settings.CSV_DB_CLIMATE_PATH, "w") as fout:
            fout.write("temp,humidity,date,room_id,app_id,device_id,device_ip\n")
    if not settings.CSV_DB_REMOTE_LOG_PATH.exists():
        with open(settings.CSV_DB_REMOTE_LOG_PATH, "w") as fout:
            fout.write("message,date,app_id,device_id,device_ip\n")
