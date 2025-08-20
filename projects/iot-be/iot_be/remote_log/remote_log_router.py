from datetime import datetime

import pydantic
import pydantic_utils
from fastapi import APIRouter, Request
from pydantic.networks import IPvAnyAddress

from . import remote_log_domain as domain
from . import remote_log_models as models

router = APIRouter(prefix="/iot")


class RemoteLogOutputSchema(pydantic_utils.BasePydanticSchema):
    # `from_attributes=True` allows this model to be build from an object with the
    #  same attr names (fi. an ORM DB model).
    model_config = pydantic.ConfigDict(from_attributes=True)
    #
    message: str | None = None
    error: str | None = None
    date: datetime
    app_id: str
    device_id: str
    device_ip: IPvAnyAddress

    @pydantic.model_validator(mode="after")
    def require_message_or_error(self):
        if not self.message and not self.error:
            raise ValueError("Either message or error is required")
        return self


class RemoteLogInputSchema(pydantic_utils.BasePydanticSchema):
    message: str | None = None
    # `error` is an optional string. However, users can also store JSON by escaping
    #  quotes, like: "{\"errorId\": \"MY_ERR_ID\"}".
    error: str | None = None
    app_id: str
    device_id: str

    @pydantic.model_validator(mode="after")
    def require_message_or_error(self):
        if not self.message and not self.error:
            raise ValueError("Either message or error is required")
        return self


# TODO add a query string like:
#  app_id=SMART_AC&device_id=ARDU_LION&date_after=2025-05-13T21:01:33.752427+02:00
@router.get("/remote-logs/")
async def read_remote_log_endpoint(n: int = 1000) -> list[RemoteLogOutputSchema]:
    log: list[models.RemoteLogDbModel] = domain.RemoteLogDomain().get_log(n)

    return [RemoteLogOutputSchema.model_validate(t) for t in log]


@router.post("/remote-logs/", status_code=201)
async def create_remote_log_endpoint(
    input_schema: RemoteLogInputSchema,
    request: Request,
) -> RemoteLogOutputSchema:
    client_ip: str = request.client.host

    log = domain.RemoteLogDomain().create_log(
        **{**input_schema.to_dict(), "device_ip": client_ip}
    )
    return RemoteLogOutputSchema.model_validate(log)
