from datetime import datetime

import pydantic
import pydantic_utils
from fastapi import APIRouter, Request
from pydantic.networks import IPvAnyAddress

from . import temp_domain as domain
from . import temp_models as models

router = APIRouter(prefix="/iot")


class TempOutputSchema(pydantic_utils.BasePydanticSchema):
    # `from_attributes=True` allows this model to be build from an object with the
    #  same attr names (fi. an ORM DB model).
    model_config = pydantic.ConfigDict(from_attributes=True)
    #
    temp: float
    humidity: float
    date: datetime
    room_id: str
    app_id: str
    device_id: str
    device_ip: IPvAnyAddress


class TempInputSchema(pydantic_utils.BasePydanticSchema):
    temp: float
    humidity: float
    room_id: str
    app_id: str
    device_id: str


# TODO add a query string like:
#  room-id=BEDROOM_PAOLO&extra-cols=device-id,ip&date_after=2025-05-13T21:01:33.752427+02:00
@router.get("/temps/")
async def read_temps_endpoint(n: int = 1000) -> list[TempOutputSchema]:
    temps: list[models.TempDbModel] = domain.TempDomain().get_temps(n)

    return [TempOutputSchema.model_validate(t) for t in temps]


@router.post("/temps/", status_code=201)
async def create_temp_endpoint(
    input_schema: TempInputSchema,
    request: Request,
) -> TempOutputSchema:
    client_ip: str = request.client.host

    temp = domain.TempDomain().create_temp(
        **{**input_schema.to_dict(), "device_ip": client_ip}
    )
    return TempOutputSchema.model_validate(temp)
